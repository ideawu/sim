#include "handler.h"
#include "../util/log.h"

namespace sim{

HandlerState Handler::accept(Link *link){
	return HANDLE_OK;
}

HandlerState Handler::close(Link *link){
	return HANDLE_OK;
}

HandlerState Handler::proc(const Request &req, Response *resp){
	return HANDLE_OK;
}

Response* Handler::handle(){
	if(this->resps.size() > 0){
		Response *resp;
		if(this->resps.pop(&resp) == 1){
			return resp;
		}
	}
	return NULL;
}

void Handler::push_response(Response *resp){
	this->resps.push(resp);
}

}; // namespace sim
