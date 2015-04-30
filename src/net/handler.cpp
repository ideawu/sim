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
	return this->pop_reponse();
}

void Handler::push_response(){
	//
}

Response* Handler::pop_response(){
	return NULL;
}

}; // namespace sim
