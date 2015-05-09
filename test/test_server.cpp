#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"
#include "sim/util/strings.h"

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
	int num = 0;
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
			resp->msg.add(str(num));
			handler->async_send(resp);
		}

		num ++;
	}
	return NULL;
}

sim::HandlerState ThreadHandler::proc(const sim::Request &req, sim::Response *resp){
	std::string cmd = req.msg.type();
	
	if(cmd == "ping"){
		resp->msg.add("ok");
		resp->msg.add("pong");
	}else if(cmd == "subscribe"){
		const std::string *token = req.msg.get(1);
		if(token && *token == "123456"){
			resp->msg.add("ok");
			resp->msg.add("subscibe successful.");
		
			Locking l(&mutex);
			sessions[req.sess.id] = req.sess;
		}else{
			resp->msg.add("error");
			resp->msg.add("invalid token!");
		}
	}else if(cmd == "echo"){
		resp->msg.add("ok");
		const std::map<int, std::string> *fields = req.msg.fields();
		std::map<int, std::string>::const_iterator it;
		for(it=fields->begin(); it!=fields->end(); it++){
			int tag = it->first;
			if(tag == 0){
				continue;
			}
			resp->msg.set(tag, it->second);
		}
	}else if(cmd == "send"){
		std::map<int64_t, sim::Session> sessions;
		{
			Locking l(&this->mutex);
			sessions = this->sessions;
		}
		std::map<int64_t, sim::Session>::iterator it;
		for(it=sessions.begin(); it!=sessions.end(); it++){
			sim::Response *resp = new sim::Response();
			resp->sess = it->second;
			resp->msg = req.msg;
			resp->msg.set_type("msg");
			this->async_send(resp);
		}
		resp->msg.add("ok");
	}else{
		resp->msg.add("client_error");
		resp->msg.add("unknown command!");
	}

	return this->resp();
}



int main(int argc, char **argv){
	//set_log_level("info");
	
	const char *ip = "127.0.0.1";
	int port = 8800;
	sim::Server *serv = sim::Server::listen(ip, port);
	if(!serv){
		log_fatal("%s", strerror(errno));
		exit(0);
	}
	log_info("server listen on %s:%d", ip, port);
	
	ThreadHandler handler;
	serv->add_handler(&handler);
	
	serv->loop();
	
	return 0;
}
