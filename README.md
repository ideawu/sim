# sim

C++ network server framework, `nc` and `telnet` friendly.


#demo

	#include "sim/sim.h"
	
	class MyHandler : public sim::Handler
	{
	public:
		virtual sim::HandlerState proc(const sim::Request &req, sim::Response *resp){
			std::string cmd = req.msg.type();
			if(cmd == "ping"){
				resp->msg.add("ok");
				resp->msg.add("pong");
			}else{
				resp->msg.add("ok");
				resp->msg.add(cmd);
			}
			return this->resp();
		}
	};
	
	int main(int argc, char **argv){
		const char *ip = "127.0.0.1";
		int port = 8800;
		sim::Server *serv = sim::Server::listen(ip, port);
		if(!serv){
			log_fatal("%s", strerror(errno));
			exit(0);
		}
		log_info("server listen on %s:%d", ip, port);
	
		MyHandler handler;
		serv->add_handler(&handler);
	
		serv->loop();
		return 0;
	}


