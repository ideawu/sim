#include <stdio.h>
#include <stdlib.h>
#include "sim.h"
#include "link.h"
#include "../util/log.h"

int main(int argc, char **argv){
	sim::Link *serv = sim::Link::listen("127.0.0.1", 8800);
	if(!serv){
		log_fatal("");
		exit(0);
	}
	while(1){
		sim::Link *link = serv->accept();
		if(!link){
			log_fatal("");
			exit(0);
		}
		log_debug("accpt link from %s:%d", link->remote_ip, link->remote_port);
	
		while(1){
			int ret;
			ret = link->read();
			if(ret == 0){
				delete link;
				log_error("link closed.");
				break;
			}
			sim::Message msg;
			ret = link->parse(&msg);
			if(ret == -1){
				delete link;
				log_error("parse error!");
				break;
			}
			if(ret == 1){
				log_debug("%s", msg.encode().c_str());
			}
		}
	}
	
	return 0;
}
