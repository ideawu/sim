#include "transport.h"
#include "util/log.h"

// using namespace sim;

int main(int argc, char **argv){
	Transport *trans = new Transport();
	trans->setup();
	log_debug("transport setup");
	
	while(1){
		usleep(100 * 1000);
	}
	return 0;
}
