#include "handler.h"

namespace sim{

HandlerState Handler::accept(Link *link){
	return HANDLE_OK;
}

HandlerState Handler::close(Link *link){
	return HANDLE_OK;
}

HandlerState Handler::proc(Link *link, const Message &msg){
	return HANDLE_OK;
}

Response* Handler::handle(){
	Response *resp;
	if(this->resps.pop(&resp) == 1){
		return resp;
	}
	return NULL;
}

void Handler::push_response(Response *resp){
	this->resps.push(resp);
}

}; // namespace sim
