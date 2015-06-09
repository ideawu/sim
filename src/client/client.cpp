#include "client.h"
#include "link.h"

namespace sim{

Client::Client(){
	link = NULL;
}

Client::~Client(){
	delete link;
}

Client* Client::connect(const char *ip, int port){
	return Client::connect(std::string(ip), port);
}

Client* Client::connect(const std::string &ip, int port){
	Link *link = Link::connect(ip, port);
	if(!link){
		return NULL;
	}
	Client *client = new Client();
	client->link = link;
	return client;
}

int Client::send(const Message &msg){
	this->link->send(msg);
	return this->link->flush();
}

int Client::recv(Message *msg){
	while(1){
		int ret;
		ret = link->recv(msg);
		if(ret == -1){
			return -1;
		}else if(ret == 1){
			return 1;
		}else{
			//
		}

		ret = link->read();
		if(ret <= 0){
			return -1;
		}
	}
}

}; // namespace sim
