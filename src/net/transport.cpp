#include "transport.h"
#include "util/log.h"
#include "line_parser.h"
#include "line_message.h"
#include "tcp_link.h"

Transport::Transport(){
}

Transport::~Transport(){
}

Event Transport::wait(int timeout_ms){
	Event event;
	_events.pop(&event, timeout_ms);
	return event;
}

void Transport::accept(int id){
	this->_accept_ids.push(id);
}

void Transport::close(int id){
	Locking l(&_mutex);
	if(_opening_list.find(id) != _opening_list.end()){
		Session *sess = _opening_list[id];
		_opening_list.erase(sess->id());
		_closing_list[sess->id()] = sess;
		
		this->_close_ids.push(id);
	}
	if(_working_list.find(id) != _working_list.end()){
		Session *sess = _working_list[id];
		_working_list.erase(sess->id());
		_closing_list[sess->id()] = sess;
		
		this->_close_ids.push(id);
	}
}

void Transport::handle_on_new(Session *sess){
	log_debug("on new %s", sess->link()->address().c_str());

	Locking l(&_mutex);
	_opening_list[sess->id()] = sess;

	this->_events.push(Event::new_event(sess));
}

void Transport::handle_on_close(Session *sess){
	log_debug("on close %s", sess->link()->address().c_str());
	this->close(sess->id());
	
	_fdes->del(sess->link()->fd());
	this->_events.push(Event::close_event(sess));
}

void Transport::handle_accept_id(){
	int id;
	_accept_ids.pop(&id);
	
	Locking l(&_mutex);
	if(_opening_list.find(id) != _opening_list.end()){
		Session *sess = _opening_list[id];
		log_debug("accept %s", sess->link()->address().c_str());
		_opening_list.erase(sess->id());
		_working_list[sess->id()] = sess;
		
		_fdes->set(sess->link()->fd(), FDEVENT_IN, 1, sess);
	}
}

void Transport::handle_close_id(){
	int id;
	_close_ids.pop(&id);

	Locking l(&_mutex);
	if(_closing_list.find(id) != _closing_list.end()){
		Session *sess = _closing_list[id];
		log_debug("close %s", sess->link()->address().c_str());
		_closing_list.erase(id);
	
		_fdes->del(sess->link()->fd());
		delete sess;
	}
}

void Transport::setup(){
	_fdes = new Fdevents();
	
	_fdes->set(_accept_ids.fd(), FDEVENT_IN, 0, &_accept_ids);
	_fdes->set(_close_ids.fd(), FDEVENT_IN, 0, &_close_ids);

	pthread_t tid;
	int err = pthread_create(&tid, NULL, &Transport::run, this);
	if(err != 0){
		log_error("can't create thread: %s", strerror(err));
	}
}

void* Transport::run(void *arg){
	Transport *trans = (Transport *)arg;
	const Fdevents::events_t *events;

	TcpLink *tcp_serv = TcpLink::listen("127.0.0.1", 8000);
	if(!tcp_serv){
		log_error("failed to listen at 127.0.0.1:8000, %s", strerror(errno));
	}
	
	trans->_fdes->set(tcp_serv->fd(), FDEVENT_IN, 0, tcp_serv);
	
	while(1){
		events = trans->_fdes->wait(500);
		
		for(int i=0; i<(int)events->size(); i++){
			const Fdevent *fde = events->at(i);
			if(fde->data.ptr == &trans->_accept_ids){
				trans->handle_accept_id();
			}else if(fde->data.ptr == &trans->_close_ids){
				trans->handle_close_id();
			}else if(fde->data.ptr == tcp_serv){
				Link *link = tcp_serv->accept();
				if(link){
					Session *sess = new Session(link, new LineParser());
					trans->handle_on_new(sess);
				}else{
					log_error("accept return NULL");
				}
			}else{
				Session *sess = (Session *)fde->data.ptr;
				if(sess){ // 防止已经被 fde_del
					log_debug("net read %s", sess->link()->address().c_str());
					Link *link = sess->link();
					int ret = link->net_read();
					if(ret <= 0){
						trans->handle_on_close(sess);
					}else{
						ParseState s = sess->parse();
						if(s.ready()){
							trans->_events.push(Event::read_event(sess));
						}else if(s.error()){
							log_debug("parse error!");
							trans->handle_on_close(sess);
						}
					}
				}
			}
		}
	}
	return NULL;
}
