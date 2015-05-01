#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"
#include "sim/log.h"
#include "sim/thread.h"

class ThreadHandler : public sim::Handler
{
public:
	ThreadHandler();
//	virtual sim::HandlerState accept(const sim::Session &sess);
	virtual sim::HandlerState close(const sim::Session &sess);
	virtual sim::HandlerState proc(const sim::Request &req, sim::Response *resp);
private:
	Mutex mutex;
	static void* _run_thread(void *arg);
	std::map<int64_t, sim::Session> sessions;
};

//sim::HandlerState ThreadHandler::accept(const sim::Session &sess){
//	return ok();
//}

sim::HandlerState ThreadHandler::close(const sim::Session &sess){
	Locking l(&mutex);
	sessions.erase(sess.id);
	return ok();
}

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
		
		std::map<int64_t, sim::Session> sessions;
		{
			Locking l(&handler->mutex);
			sessions = handler->sessions;
		}
		std::map<int64_t, sim::Session>::iterator it;
		for(it=sessions.begin(); it!=sessions.end(); it++){
			sim::Response *resp = new sim::Response();
			resp->sess = it->second;
			resp->msg.add("timer");
			handler->async_send(resp);
		}
	}
	return NULL;
}

sim::HandlerState ThreadHandler::proc(const sim::Request &req, sim::Response *resp){
	const std::string *cmd = req.msg.get(0);
	const std::string *token = req.msg.get(1);
	if(cmd && token && *cmd == "subscribe" && *token == "123456"){
		resp->msg.add("ok");
		resp->msg.add("subscibe successful.");
		
		Locking l(&mutex);
		sessions[req.sess.id] = req.sess;
	}else{
		if(cmd && *cmd == "subscribe"){
			resp->msg.add("error");
			resp->msg.add("invalid token!");
		}else{
			resp->msg.add("error");
			resp->msg.add("unknown command!");
		}
	}

	return this->resp();
}



int main(int argc, char **argv){
	const char *ip = "127.0.0.1";
	int port = 8800;
	sim::Server *serv = sim::Server::listen(ip, port);
	if(!serv){
		log_fatal("");
		exit(0);
	}
	log_info("server listen on %s:%d", ip, port);
	
	ThreadHandler handler;
	serv->add_handler(&handler);
	
	serv->loop();
	
	return 0;
}
