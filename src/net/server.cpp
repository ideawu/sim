#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "server.h"
#include "../util/log.h"

namespace sim{
	
const static int DEFAULT_TYPE = 0;
const static int HANDLER_TYPE = 1;
	
static inline double millitime(){
	struct timeval now;
	gettimeofday(&now, NULL);
	double ret = now.tv_sec + now.tv_usec/1000.0/1000.0;
	return ret;
}

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
	return ret;
}

void Server::add_handler(Handler *handler){
	this->handlers.push_back(handler);
	if(handler->fd() > 0){
		fdes->set(handler->fd(), FDEVENT_IN, HANDLER_TYPE, handler);
	}
}

Link* Server::accept_link(){
	Link *link = serv_link->accept();
	if(link == NULL){
		log_error("accept failed! %s", strerror(errno));
		return NULL;
	}
				
	link->nodelay();
	link->noblock();
	link->create_time = millitime();
	link->active_time = link->create_time;
	
	for(int i=0; i<this->handlers.size(); i++){
		Handler *handler = this->handlers[i];
		HandlerState state = handler->accept(link);
		if(state == HANDLE_FAIL){
			delete link;
			return NULL;
		}
	}
	
	this->link_count ++;				
	log_debug("new link from %s:%d, fd: %d, links: %d",
		link->remote_ip, link->remote_port, link->fd(), this->link_count);
	fdes->set(link->fd(), FDEVENT_IN, DEFAULT_TYPE, link);
	
	return link;
}

int Server::close_link(Link *link){
	for(int i=0; i<this->handlers.size(); i++){
		Handler *handler = this->handlers[i];
		handler->close(link);
	}
	
	this->link_count --;
	log_debug("delete link %s:%d, fd: %d, links: %d",
		link->remote_ip, link->remote_port, link->fd(), this->link_count);
	fdes->del(link->fd());
	delete link;
	return 0;
}

int Server::read_link(Link *link){
	if(link->error()){
		return 0;
	}
	
	int len = link->read();
	if(len <= 0){
		log_debug("fd: %d, read: %d, delete link", link->fd(), len);
		this->close_link(link);
		return -1;
	}
	
	while(1){
		Message msg;
		int ret = link->parse(&msg);
		if(ret == -1){
			log_info("fd: %d, parse error, delete link", link->fd());
			this->close_link(link);
			return -1;
		}else if(ret == 0){
			// 报文未就绪, 继续读网络
			break;
		}

		for(int i=0; i<this->handlers.size(); i++){
			Handler *handler = this->handlers[i];
			HandlerState state = handler->proc(link, msg);
			if(state == HANDLE_RESP){
				//
			}else if(state == HANDLE_FAIL){
				this->close_link(link);
				return -1;
			}
		}
	}

	return 0;
}

int Server::write_link(Link *link){
	if(link->error()){
		return 0;
	}

	int len = link->write();
	if(len <= 0){
		log_debug("fd: %d, write: %d, delete link", link->fd(), len);
		this->close_link(link);
		return -1;
	}
	if(link->output.empty()){
		fdes->clr(link->fd(), FDEVENT_OUT);
	}
	return 0;
}

void Server::loop(){
	fdes->set(serv_link->fd(), FDEVENT_IN, DEFAULT_TYPE, serv_link);

	while(1){
		const Fdevents::events_t *events;
		if(!this->links.empty()){
			// ready_list not empty, so we should return immediately
			events = fdes->wait(0);
		}else{
			events = fdes->wait(20);
		}
		if(events == NULL){
			log_fatal("events.wait error: %s", strerror(errno));
			break;
		}
		
		for(int i=0; i<(int)events->size(); i++){
			const Fdevent *fde = events->at(i);
			Link *link = NULL;
			if(fde->data.ptr == serv_link){
				link = this->accept_link();
			}else if(fde->data.num == HANDLER_TYPE){
				Handler *handler = (Handler *)fde->data.ptr;
				/*
				while(1){
					resp = handler->read();
					resp->link->send(resp->msg);
					delete resp;
				}
				*/
			}else{
				Link *link = (Link *)fde->data.ptr;
				if(fde->events & FDEVENT_IN){
					if(this->read_link(link) == -1){
						continue;
					}
				}
				if(fde->events & FDEVENT_OUT){
					if(this->write_link(link) == -1){
						continue;
					}
				}
			}
			if(link && !link->output.empty()){
				fdes->set(link->fd(), FDEVENT_OUT, DEFAULT_TYPE, link);
			}
		}
	}
}

}; // namespace sim
