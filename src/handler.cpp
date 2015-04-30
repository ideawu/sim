#include "handler.h"
#include "server.h"
#include "log.h"

namespace sim{

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
	this->resps.push(resp);
}

Response* Handler::handle(){
	while(this->resps.size() > 0){
		Response *resp;
		if(this->resps.pop(&resp) == 1 && resp != NULL){
			return resp;
		}
	}
	return NULL;
}

}; // namespace sim
