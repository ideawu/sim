#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "util/log.h"
#include "sim.h"
#include "fde.h"
#include "server.h"

#include "util/strings.h"

static std::string msg_str(const sim::Message &msg){
	std::string ret;
	const std::map<int, std::string> *fields = msg.fields();
	std::map<int, std::string>::const_iterator it;
	char buf[50];
	int count = 0;
	for(it=fields->begin(); it!=fields->end(); it++){
		if(ret.size() > 100){
			snprintf(buf, sizeof(buf), "[%d more...]", (int)fields->size() - count);
			ret.append(buf);
			break;
		}
		
		int tag = it->first;
		const std::string &val = it->second;
		ret.append(str(tag));
		ret.push_back('=');
		if(val.size() < 30){
			std::string h = sim::encode(val, true);
			ret.append(h);
		}else{
			sprintf(buf, "[%d]", (int)val.size());
			ret.append(buf);
		}
		
		count ++;
		if(count != (int)fields->size()){
			ret.push_back(' ');
		}
	}
	return ret;
}


namespace sim{
	
const static int DEFAULT_TYPE = 0;
const static int HANDLER_TYPE = 1;

Server::Server(){
	signal(SIGPIPE, SIG_IGN);
	link_count = 0;
	serv_link = NULL;
	fdes = new Fdevents();
}

Server::~Server(){
	for(int i=0; i<this->handlers.size(); i++){
		Handler *handler = this->handlers[i];
		delete handler;
	}
	this->handlers.clear();
	
	delete serv_link;
	delete fdes;
}

Server* Server::listen(const std::string &ip, int port){
	Link *serv_link = Link::listen(ip, port);
	if(!serv_link){
		return NULL;
	}
	
	Server *ret = new Server();
	ret->serv_link = serv_link;
	ret->fdes->set(serv_link->fd(), FDEVENT_IN, DEFAULT_TYPE, serv_link);
	return ret;
}

void Server::add_handler(Handler *handler){
	this->handlers.push_back(handler);
	if(handler->fd() > 0){
		fdes->set(handler->fd(), FDEVENT_IN, HANDLER_TYPE, handler);
	}
}

Session* Server::accept_session(){
	Link *link = serv_link->accept();
	if(link == NULL){
		log_error("accept failed! %s", strerror(errno));
		return NULL;
	}
				
	link->nodelay();
	link->noblock();
	link->create_time = millitime();
	link->active_time = link->create_time;
	
	Session *sess = new Session();
	sess->link = link;
	this->sessions[sess->id] = sess;
	
	for(int i=0; i<this->handlers.size(); i++){
		Handler *handler = this->handlers[i];
		HandlerState state = handler->accept(*sess);
		if(state == HANDLE_FAIL){
			delete link;
			delete sess;
			return NULL;
		}
	}
	
	this->link_count ++;				
	log_debug("new link from %s:%d, fd: %d, links: %d",
		link->remote_ip, link->remote_port, link->fd(), this->link_count);
	fdes->set(link->fd(), FDEVENT_IN, DEFAULT_TYPE, sess);
	
	return sess;
}

int Server::close_session(Session *sess){
	Link *link = sess->link;
	for(int i=0; i<this->handlers.size(); i++){
		Handler *handler = this->handlers[i];
		handler->close(*sess);
	}
	
	this->link_count --;
	log_debug("delete link %s:%d, fd: %d, links: %d",
		link->remote_ip, link->remote_port, link->fd(), this->link_count);
	fdes->del(link->fd());

	this->sessions.erase(sess->id);
	delete link;
	delete sess;
	return 0;
}

int Server::read_session(Session *sess){
	Link *link = sess->link;
	if(link->error()){
		return 0;
	}
	
	int len = link->read();
	if(len <= 0){
		this->close_session(sess);
		return -1;
	}
	
	while(1){
		Request req;
		int ret = link->recv(&req.msg);
		if(ret == -1){
			log_info("fd: %d, parse error, delete link", link->fd());
			this->close_session(sess);
			return -1;
		}else if(ret == 0){
			// 报文未就绪, 继续读网络
			break;
		}
		req.stime = millitime();
		req.sess = *sess;

		Response resp;
		for(int i=0; i<this->handlers.size(); i++){
			Handler *handler = this->handlers[i];
			req.time_wait = 1000 * (millitime() - req.stime);
			HandlerState state = handler->proc(req, &resp);
			req.time_proc = 1000 * (millitime() - req.stime) - req.time_wait;
			if(state == HANDLE_RESP){
				link->send(resp.msg);
				if(link && !link->output.empty()){
					fdes->set(link->fd(), FDEVENT_OUT, DEFAULT_TYPE, sess);
				}
				
				if(log_level() >= Logger::LEVEL_DEBUG){
					log_debug("w:%.3f,p:%.3f, req: %s resp: %s",
						req.time_wait, req.time_proc,
						msg_str(req.msg).c_str(),
						msg_str(resp.msg).c_str());
				}
			}else if(state == HANDLE_FAIL){
				this->close_session(sess);
				return -1;
			}
		}
	}

	return 0;
}

int Server::write_session(Session *sess){
	Link *link = sess->link;
	if(link->error()){
		return 0;
	}

	int len = link->write();
	if(len <= 0){
		log_debug("fd: %d, write: %d, delete link", link->fd(), len);
		this->close_session(sess);
		return -1;
	}
	if(link->output.empty()){
		fdes->clr(link->fd(), FDEVENT_OUT);
	}
	return 0;
}

Session* Server::get_session(int64_t sess_id){
	std::map<int64_t, Session *>::iterator it;
	it = sessions.find(sess_id);
	if(it == sessions.end()){
		return NULL;
	}
	return it->second;
}

void Server::loop(){
	while(1){
		if(this->loop_once() == -1){
			break;
		}
	}
}

int Server::loop_once(){
	const Fdevents::events_t *events;
	events = fdes->wait(20);
	if(events == NULL){
		log_fatal("events.wait error: %s", strerror(errno));
		return 0;
	}
	
	for(int i=0; i<(int)events->size(); i++){
		const Fdevent *fde = events->at(i);
		if(fde->data.ptr == serv_link){
			this->accept_session();
		}else if(fde->data.num == HANDLER_TYPE){
			Handler *handler = (Handler *)fde->data.ptr;
			while(Response *resp = handler->handle()){
				Session *sess = this->get_session(resp->sess.id);
				if(sess){
					Link *link = sess->link;
					link->send(resp->msg);
					if(link && !link->output.empty()){
						fdes->set(link->fd(), FDEVENT_OUT, DEFAULT_TYPE, sess);
					}
				}
				delete resp;
			}
		}else{
			Session *sess = (Session *)fde->data.ptr;
			Link *link = sess->link;
			if(fde->events & FDEVENT_IN){
				if(this->read_session(sess) == -1){
					continue;
				}
			}
			if(fde->events & FDEVENT_OUT){
				if(this->write_session(sess) == -1){
					continue;
				}
			}
			if(link && !link->output.empty()){
				fdes->set(link->fd(), FDEVENT_OUT, DEFAULT_TYPE, sess);
			}
		}
	}
	return 0;
}

}; // namespace sim
