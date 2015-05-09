#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"

int main(int argc, char **argv){
	sim::Link *link = sim::Link::connect("127.0.0.1", 8800);
	if(!link){
		log_fatal("%s", strerror(errno));
		exit(0);
	}
	
	sim::Message msg;
	msg.add("ping");
	msg.add("hello world!");
	
	link->send(msg);
	link->flush();

	while(1){
		int ret;
		ret = link->read();
		log_debug("read %d", ret);
		if(ret == 0){
			exit(0);
		}
		sim::Message msg;
		ret = link->recv(&msg);
		if(ret == -1){
			exit(0);
		}else if(ret == 0){
			continue;
		}
		log_debug("received msg: %s", msg.encode().c_str());
		break;
	}
	
	return 0;
}
