#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"
#include "sim/fde.h"

int main(int argc, char **argv){
	set_log_level("info");
	
	sim::Link *link = sim::Link::connect("127.0.0.1", 8800);
	if(!link){
		log_fatal("");
		exit(0);
	}

	Fdevents *fdes;
	fdes = new Fdevents();

	fdes->set(link->fd(), FDEVENT_IN|FDEVENT_OUT, 0, link);
	
	sim::Message msg;
	msg.add("ping");
	msg.add("hello world!");
	std::string msg_str = msg.encode();
	
	int total_reqs = 0;
	if(argc > 1){
		total_reqs = atoi(argv[1]);
	}
	total_reqs = total_reqs > 0? total_reqs : 1000;
	
	int sent_reqs = 0;
	int recv_reqs = 0;
	
	double stime = sim::millitime();

	while(1){
		const Fdevents::events_t *events;
		events = fdes->wait(50);
		if(events == NULL){
			log_error("events.wait error: %s", strerror(errno));
			break;
		}

		for(int i=0; i<(int)events->size(); i++){
			const Fdevent *fde = events->at(i);
			sim::Link *link = (sim::Link *)fde->data.ptr;
			if(fde->events & FDEVENT_IN){
				int ret;
				ret = link->read();
				log_debug("read %d", ret);
				if(ret == 0){
					delete link;
					log_error("link closed.");
					exit(0);
				}
				while(1){
					sim::Message msg;
					ret = link->recv(&msg);
					if(ret == -1){
						delete link;
						log_error("parse error!");
						exit(0);
					}else if(ret == 0){
						break;
					}
					recv_reqs ++;
					if(recv_reqs % 1000 == 1 || recv_reqs == total_reqs){
						log_debug("received %d message(s)", recv_reqs);
					}
					//printf("recv: %s", msg.encode().c_str());
					if(recv_reqs == total_reqs){
						double etime = sim::millitime();
						double ts = etime - stime;
						int qps = total_reqs / ts;
						log_info("recv all, time: %.2f ms, %d qps", 1000*ts, qps);
						exit(0);
					}
				}
			}
			if(fde->events & FDEVENT_OUT){
				sent_reqs ++;
				if(sent_reqs <= total_reqs){
					//log_debug("send %d", sent_reqs);
					link->output.append(msg_str);
					if(sent_reqs % 1000 == 1 || sent_reqs == total_reqs){
						log_debug("sent %d message(s)", sent_reqs);
					}
				}
				if(link->output.empty()){
					double etime = sim::millitime();
					double ts = etime - stime;
					int qps = total_reqs / ts;
					log_info("sent all, time: %.2f s, %d qps", 1000*ts, qps);
					fdes->clr(link->fd(), FDEVENT_OUT);
				}
				link->write();
				//log_debug("write %d", ret);
			}
		}
	}
	
	return 0;
}
