#include "transport_impl.h"
#include "util/log.h"
#include "net/fde.h"
#include "net/link.h"

#define FDE_NUM_COMMON 0
#define FDE_NUM_SERVER 1
#define FDE_NUM_CLIENT 2

// static
Transport* Transport::create(){
	Transport *ret = new TransportImpl();
	return ret;
}

TransportImpl::TransportImpl(){
	_fdes = new Fdevents();
}

TransportImpl::~TransportImpl(){
	delete _fdes;
}

void TransportImpl::add_server(Server *serv){
	serv->init();
	_servers.push_back(serv);
	_fdes->set(serv->link()->fd(), FDEVENT_IN, FDE_NUM_SERVER, serv);
}

void TransportImpl::init(){
	_fdes->set(_accept_ids.fd(), FDEVENT_IN, FDE_NUM_COMMON, &_accept_ids);
	_fdes->set(_close_ids.fd(), FDEVENT_IN, FDE_NUM_COMMON, &_close_ids);
	_fdes->set(_send_ids.fd(), FDEVENT_IN, FDE_NUM_COMMON, &_send_ids);

}

void TransportImpl::accept(int id){
	this->_accept_ids.push(id);
}

void TransportImpl::close(int id){
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

Message* TransportImpl::recv(int id){
	Locking l(&_mutex);
	if(_working_list.find(id) != _working_list.end()){
		Session *sess = _working_list[id];
		return sess->recv();
	}
	return NULL;
}

void TransportImpl::handle_on_read(Session *sess){
	// log_debug("net read %s", sess->link()->address().c_str());
	
	bool error = false;
	int ret = sess->link()->net_read();
	if(ret <= 0){
		error = true;
	}else{
		Locking l(&_mutex);
		int num = sess->parse_input();
		if(num == -1){
			log_debug("parse error!");
			error = true;
		}else{
			// log_debug("parsed %d message(s)", num);
			for(int i=0; i<num; i++){
				this->_events.push_back(Event::read_event(sess));
			}
		}
	}

	if(error){
		this->handle_on_close(sess);
	}
}

void TransportImpl::send(int id, Message *msg){
	Locking l(&_mutex);
	if(_working_list.find(id) != _working_list.end()){
		Session *sess = _working_list[id];
		sess->send(msg);

		// log_debug("output.size %d", sess->output()->size());
		this->_send_ids.push(id);
	}
}

void TransportImpl::handle_send_id(){
	Locking l(&_mutex);

	int id;
	_send_ids.pop(&id);

	if(_working_list.find(id) != _working_list.end()){
		Session *sess = _working_list[id];
		if(!sess->output()->empty() && !_fdes->isset(sess->link()->fd(), FDEVENT_OUT)){
			// log_debug("fde.set(%d, OUT)", sess->id());
			_fdes->set(sess->link()->fd(), FDEVENT_OUT, FDE_NUM_CLIENT, sess);
		}
	}
}

void TransportImpl::handle_on_write(Session *sess){
	bool error = false;

	{
		Locking l(&_mutex);
		if(!sess->output()->empty()){
			int num = sess->encode_output();
			if(num == -1){
				log_debug("encode error!");
				error = true;
			}else{
				// log_debug("encoded %d message(s)", num);
			}
		}
	}
	
	int ret = sess->link()->net_write();
	if(ret == 0){
		// log_debug("fde.clr(%d, OUT)", sess->id());
		_fdes->clr(sess->link()->fd(), FDEVENT_OUT);
	}else if(ret == -1){
		error = true;
	}

	if(error){
		this->handle_on_close(sess);
	}
}

void TransportImpl::handle_on_new(Session *sess){
	log_debug("on new %s", sess->link()->address().c_str());

	Locking l(&_mutex);
	_opening_list[sess->id()] = sess;

	this->_events.push_back(Event::new_event(sess));
}

void TransportImpl::handle_on_close(Session *sess){
	int id = sess->id();
	log_debug("on close %s", sess->link()->address().c_str());
	Locking l(&_mutex);
	if(_working_list.find(id) != _working_list.end()){
		_working_list.erase(sess->id());
		_closing_list[sess->id()] = sess;
	
		_fdes->del(sess->link()->fd());
		this->_events.push_back(Event::close_event(sess));
	}
}

void TransportImpl::handle_accept_id(){
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

void TransportImpl::handle_close_id(){
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

const std::vector<Event>* TransportImpl::wait(int timeout_ms){
	_events.clear();

	const Fdevents::events_t *events = _fdes->wait(timeout_ms);
	for(int i=0; i<(int)events->size(); i++){
		const Fdevent *fde = events->at(i);
		if(fde->data.ptr == &this->_accept_ids){
			this->handle_accept_id();
		}else if(fde->data.ptr == &this->_close_ids){
			this->handle_close_id();
		}else if(fde->data.ptr == &this->_send_ids){
			this->handle_send_id();
		}else{
			if(fde->data.num == FDE_NUM_SERVER){
				Server *serv = (Server *)fde->data.ptr;
				Session *sess = serv->accept();
				if(sess){
					this->handle_on_new(sess);
				}else{
					log_error("accept return NULL");
				}
			}else{
				Session *sess = (Session *)fde->data.ptr;
				if(sess){ // 防止已经被 fde_del
					if(fde->events & FDEVENT_IN){
						this->handle_on_read(sess);
					}
					if(fde->events & FDEVENT_OUT){
						this->handle_on_write(sess);
					}
				}
			}
		}
	}
	
	return &_events;
}
