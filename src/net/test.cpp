#include "transport.h"
#include "util/log.h"

// using namespace sim;

#define TICK_INTERVAL          100 // ms
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
#ifndef __CYGWIN__
	signal(SIGALRM, signal_handler);
	{
		struct itimerval tv;
		tv.it_interval.tv_sec = (TICK_INTERVAL / 1000);
		tv.it_interval.tv_usec = (TICK_INTERVAL % 1000) * 1000;
		tv.it_value.tv_sec = 1;
		tv.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &tv, NULL);
	}
#endif
	
	Transport *trans = new Transport();
	trans->setup();
	log_debug("transport setup");
	
	TcpLink *link = trans->wait();
	if(link->is_new()){
		trans->read(link);
	}else if(link->is_closing()){
		trans->close(link);
	}
	
	while(!quit){
		usleep(100 * 1000);
	}
	return 0;
}
