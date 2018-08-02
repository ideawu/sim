#include "util/log.h"
#include "core/event.h"
#include "core/transport.h"
#include "line/line_message.h"
#include "line/line_server.h"

// using namespace sim;

#define TICK_INTERVAL          1000 // ms
volatile bool quit = false;

void signal_handler(int sig){
	switch(sig){
		case SIGTERM:
		case SIGINT:{
			quit = true;
			break;
		}
		case SIGALRM:{
			break;
		}
	}
}

int main(int argc, char **argv){
	// set_log_level("error");
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
// #ifndef __CYGWIN__
// 	signal(SIGALRM, signal_handler);
// 	{
// 		struct itimerval tv;
// 		tv.it_interval.tv_sec = (TICK_INTERVAL / 1000);
// 		tv.it_interval.tv_usec = (TICK_INTERVAL % 1000) * 1000;
// 		tv.it_value.tv_sec = 0;
// 		tv.it_value.tv_usec = TICK_INTERVAL * 1000;
// 		setitimer(ITIMER_REAL, &tv, NULL);
// 	}
// #endif
	
	const char *host = "127.0.0.1";
	int port = 8000;
	
	Server *serv = new LineServer();
	if(serv->listen(host, port) == -1){
		log_error("failed to listen at %s:%d, %s", host, port, strerror(errno));
		exit(0);
	}
	log_debug("server listen at %s:%d", host, port);
	
	Transport *trans = Transport::create();
	trans->add_server(serv);
	trans->init();
	log_debug("transport setup");

	while(!quit){
		const std::vector<Event> *events = trans->wait(200);
		for(int i=0; i<(int)events->size(); i++){
			Event event = events->at(i);
		
			if(event.is_new()){
				log_debug("accept");
				trans->accept(event.id());
			}else if(event.is_close()){
				log_debug("close");
				trans->close(event.id());
			}else if(event.is_read()){
				// log_debug("read");
				LineMessage *req = (LineMessage *)trans->recv(event.id());
				if(!req){
					// do nothing, the close event will finally be triggered
					log_debug("recv NULL msg, detect session closed");
				}else{
					log_debug("recv: %s", req->text().c_str());
				
					std::string text = "req=";
					text.append(req->text());
					LineMessage *resp = new LineMessage();
					resp->text(text);
					trans->send(event.id(), resp);
				
					delete req;
				}
			}
		}
	}
	
	return 0;
}
