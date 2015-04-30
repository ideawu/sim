#include <stdio.h>
#include <stdlib.h>
#include "sim.h"
#include "../util/log.h"
#include "../util/thread.h"

class ThreadHandler : public sim::Handler
{
public:
	ThreadHandler();
	virtual sim::HandlerState accept(sim::Link *link);
	virtual sim::HandlerState close(sim::Link *link);
	virtual sim::HandlerState proc(const sim::Request &req, sim::Response *resp);
private:
	std::map<int, sim::Link *> links;
	static void* _run_thread(void *arg);
	Mutex mutex;
};

ThreadHandler::ThreadHandler(){
	pthread_t tid;
	int err = pthread_create(&tid, NULL, &ThreadHandler::_run_thread, this);
	if(err != 0){
		log_error("can't create thread: %s", strerror(err));
	}
}

void* ThreadHandler::_run_thread(void *arg){
	ThreadHandler *handler = (ThreadHandler *)arg;
	while(1){
		sleep(2);
		
		std::map<int, sim::Link *> links;
		{
			Locking(&handler->mutex);
			links = handler->links;
		}
		std::map<int, sim::Link *>::iterator it;
		for(it=links.begin(); it!=links.end(); it++){
			sim::Response *resp = new sim::Response();
			resp->link = it->second;
			resp->msg.add("timer");
			handler->push_response(resp);
		}
	}
	return NULL;
}

sim::HandlerState ThreadHandler::accept(sim::Link *link){
	Locking(&this->mutex);
	links[link->fd()] = link;
	return ok();
}

sim::HandlerState ThreadHandler::close(sim::Link *link){
	Locking(&this->mutex);
	links.erase(link->fd());
	return ok();
}

sim::HandlerState ThreadHandler::proc(const sim::Request &req, sim::Response *resp){
	resp->msg.add("ok");
	return this->resp();
}



int main(int argc, char **argv){
	sim::Server *serv = sim::Server::listen("127.0.0.1", 8800);
	if(!serv){
		log_fatal("");
		exit(0);
	}
	
	ThreadHandler handler;
	serv->add_handler(&handler);
	
	serv->loop();
	
	return 0;
}
