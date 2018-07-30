#include "transport.h"

Transport::Transport(){
}

Transport::~Transport(){
}

Link* Transport::listen(){
	while(1){
		_signal.wait();
		Locking l(&_mutex);
		
		if(!_waiting_links.empty()){
			Link *link = _waiting_links.front();
			_waiting_links.pop_front();
			return link;
		}
	}
}

void Transport::accept(Link *link){
	Locking l(&_mutex);
	
	_working_links.insert(link);
	link->accept();
}

void Transport::close(Link *link){
	Locking l(&_mutex);

	_working_links.erase(link);
	link->close();
}

void Transport::setup(){
	
}

void* Transport::run(void *arg){
	
}
