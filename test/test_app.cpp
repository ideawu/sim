#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"

class MyApplication : public sim::Application
{
public:
	virtual void run();
};

void MyApplication::run(){
	log_info("config : %s", this->app_args.conf_file.c_str());
	log_info("pidfile: %s, pid: %d", app_args.pidfile.c_str(), (int)getpid());
	log_info("logger");
	log_info("    level : %s", Logger::shared()->level_name().c_str());
	log_info("    output: %s", Logger::shared()->output_name().c_str());
	log_info("    rotate: %" PRId64, Logger::shared()->rotate_size());
	log_info("server started.");
	
	while(this->running()){
		sleep(1);
		log_debug("");
	}

	log_info("server exit.");
}

int main(int argc, char **argv){
	MyApplication app;
	return app.main(argc, argv);
}
