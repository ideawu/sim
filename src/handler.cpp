#include "util/log.h"
#include "handler.h"
#include "util/thread.h"

namespace sim{

int Handler::m_init(){
	resps = new SelectableQueue<Response *>();
	return this->init();
}

int Handler::m_free(){
	delete resps;
	return this->free();
}

int Handler::fd(){
	return resps->fd();
}

HandlerState Handler::accept(const Session &sess){
	return HANDLE_OK;
}

HandlerState Handler::close(const Session &sess){
	return HANDLE_OK;
}

HandlerState Handler::proc(const Request &req, Response *resp){
	return HANDLE_OK;
}

void Handler::async_send(Response *resp){
	this->resps->push(resp);
}

Response* Handler::handle(){
	while(this->resps->size() > 0){
		Response *resp;
		if(this->resps->pop(&resp) == 1 && resp != NULL){
			return resp;
		}
	}
	return NULL;
}

}; // namespace sim
