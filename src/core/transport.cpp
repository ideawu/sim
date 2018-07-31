#include "transport.h"
#include "util/log.h"
#include "net/fde.h"
#include "net/link.h"

#define FDE_NUM_COMMON 0
#define FDE_NUM_SERVER 1
#define FDE_NUM_CLIENT 2

Transport::Transport(){
	_fdes = new Fdevents();
}

Transport::~Transport(){
	delete _fdes;
}

void Transport::add_server(Server *serv){
	serv->init();
	_servers.push_back(serv);
	_fdes->set(serv->link()->fd(), FDEVENT_IN, FDE_NUM_SERVER, serv);
}

void Transport::setup(){
	_fdes->set(_accept_ids.fd(), FDEVENT_IN, FDE_NUM_COMMON, &_accept_ids);
	_fdes->set(_close_ids.fd(), FDEVENT_IN, FDE_NUM_COMMON, &_close_ids);

	pthread_t tid;
	int err = pthread_create(&tid, NULL, &Transport::run, this);
	if(err != 0){
		log_error("can't create thread: %s", strerror(err));
	}
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
	}
	if(_working_list.find(id) != _working_list.end()){
		Session *sess = _working_list[id];
		_working_list.erase(sess->id());
		_closing_list[sess->id()] = sess;
	}
	
	this->_close_ids.push(id);
}

Message* Transport::recv(int id){
	Locking l(&_mutex);
	if(_working_list.find(id) != _working_list.end()){
		Session *sess = _working_list[id];
		return sess->recv();
	}
	return NULL;
}

void Transport::handle_on_new(Session *sess){
	log_debug("on new %s", sess->link()->address().c_str());

	Locking l(&_mutex);
	_opening_list[sess->id()] = sess;

	this->_events.push(Event::new_event(sess));
}

void Transport::handle_on_close(Session *sess){
	int id = sess->id();
	log_debug("on close %s", sess->link()->address().c_str());
	Locking l(&_mutex);
	if(_working_list.find(id) != _working_list.end()){
		_working_list.erase(sess->id());
		_closing_list[sess->id()] = sess;
	
		_fdes->del(sess->link()->fd());
		this->_events.push(Event::close_event(sess));
	}
}

void Transport::handle_on_read(Session *sess){
	// log_debug("net read %s", sess->link()->address().c_str());
	
	bool error = false;
	int ret = sess->link()->net_read();
	if(ret <= 0){
		error = true;
	}else{
		Locking l(&_mutex);
		int num = sess->parse();
		if(num == -1){
			log_debug("parse error!");
			error = true;
		}else{
			for(int i=0; i<num; i++){
				this->_events.push(Event::read_event(sess));
			}
		}
	}

	if(error){
		this->handle_on_close(sess);
	}
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
		
		_fdes->set(sess->link()->fd(), FDEVENT_IN, FDE_NUM_CLIENT, sess);
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

void* Transport::run(void *arg){
	Transport *trans = (Transport *)arg;
	const Fdevents::events_t *events;
	
	while(1){
		events = trans->_fdes->wait(500);
		
		for(int i=0; i<(int)events->size(); i++){
			const Fdevent *fde = events->at(i);
			if(fde->data.ptr == &trans->_accept_ids){
				trans->handle_accept_id();
			}else if(fde->data.ptr == &trans->_close_ids){
				trans->handle_close_id();
			}else{
				if(fde->data.num == FDE_NUM_SERVER){
					Server *serv = (Server *)fde->data.ptr;
					Session *sess = serv->accept();
					if(sess){
						trans->handle_on_new(sess);
					}else{
						log_error("accept return NULL");
					}
				}else{
					Session *sess = (Session *)fde->data.ptr;
					if(sess){ // 防止已经被 fde_del
						trans->handle_on_read(sess);
					}
				}
			}
		}
	}
	return NULL;
}
