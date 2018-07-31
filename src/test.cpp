#include "transport.h"
#include "util/log.h"
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
	
	Transport *trans = new Transport();
	trans->add_server(serv);
	trans->setup();
	log_debug("transport setup");

	while(!quit){
		Event event = trans->wait(200);
		if(event.is_new()){
			log_debug("accept");
			trans->accept(event.id());
		}else if(event.is_close()){
			log_debug("close");
			trans->close(event.id());
		}else if(event.is_read()){
			// log_debug("read");
			LineMessage *msg = (LineMessage *)trans->recv(event.id());
			if(!msg){
				// do nothing, the close event will finally be triggered
				log_debug("recv NULL msg, detect session closed");
			}else{
				log_debug("recv: %s", msg->text().c_str());
				delete msg;
			}
		}
	}
	
	return 0;
}
