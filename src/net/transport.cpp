#include "transport.h"
#include "util/log.h"

Transport::Transport(){
}

Transport::~Transport(){
}

LinkEvent Transport::wait(int timeout_ms){
	LinkEvent event;
	_events.pop(&event, timeout_ms);
	return event;
}

void Transport::accept(int id){
	this->_accept_ids.push(id);
}

void Transport::close(int id){
	Locking l(&_mutex);
	if(_opening_list.find(id) != _opening_list.end()){
		Link *link = _opening_list[id];
		_opening_list.erase(link->id());
		_closing_list[link->id()] = link;
		
		this->_close_ids.push(id);
	}
	if(_working_list.find(id) != _working_list.end()){
		Link *link = _working_list[id];
		_working_list.erase(link->id());
		_closing_list[link->id()] = link;
		
		this->_close_ids.push(id);
	}
}

void Transport::handle_on_new(Link *link){
	Locking l(&_mutex);
	_opening_list[link->id()] = link;

	this->_events.push(LinkEvent::new_link(link));
}

void Transport::handle_on_close(Link *link){
	this->close(link->id());
	
	_fdes->del(link->fd());
	this->_events.push(LinkEvent::close_link(link));
}

void Transport::handle_accept_id(){
	int id;
	_accept_ids.pop(&id);
	
	Locking l(&_mutex);
	if(_opening_list.find(id) != _opening_list.end()){
		Link *link = _opening_list[id];
		log_debug("accept %s:%d", link->remote_ip, link->remote_port);
		_opening_list.erase(link->id());
		_working_list[link->id()] = link;
		
		_fdes->set(link->fd(), FDEVENT_IN, 1, link);
	}
}

void Transport::handle_close_id(){
	int id;
	_close_ids.pop(&id);

	Locking l(&_mutex);
	if(_closing_list.find(id) != _closing_list.end()){
		Link *link = _closing_list[id];
		log_debug("close %s:%d", link->remote_ip, link->remote_port);
		_closing_list.erase(id);
	
		_fdes->del(link->fd());
		delete link;
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
				TcpLink *link = tcp_serv->accept();
				if(link){
					log_debug("on new %s:%d", link->remote_ip, link->remote_port);
					trans->handle_on_new(link);
				}else{
					log_error("accept return NULL");
				}
			}else{
				TcpLink *link = (TcpLink *)fde->data.ptr;
				if(link){ // 防止已经被 fde_del
					log_debug("on read %s:%d", link->remote_ip, link->remote_port);
					int ret = link->net_read();
					if(ret <= 0){
						log_debug("on close %s:%d", link->remote_ip, link->remote_port);
						trans->handle_on_close(link);
					}else{
						//
					}
				}
			}
		}
	}
	return NULL;
}
