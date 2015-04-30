#include "handler.h"
#include "log.h"

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

HandlerState Handler::on_accept(Link *link){
	Locking l(&this->mutex);
	links[link->fd()] = link;
	return this->accept(link);
}

HandlerState Handler::on_close(Link *link){
	Locking l(&this->mutex);
	links.erase(link->fd());
	
	{
		Locking l(&resps.mutex);
		// 把 message queue 中的所有该 link 相关的 response 删除
		std::list<Response *> &rs = this->resps.items;
		std::list<Response *>::iterator it;
		for(it = rs.begin(); it!=rs.end(); it++){
			Response *resp = *it;
			if(resp->link->fd() == link->fd()){
				delete resp;
				*it = NULL;
			}
		}
	}
	
	return this->close(link);
}

void Handler::async_send(Response *resp){
	Locking(&this->mutex);
	std::map<int, Link *>::iterator it;
	it = links.find(resp->link->fd());
	if(it == links.end() || it->second != resp->link){
		return;
	}
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
