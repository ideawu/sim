#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"

class MyApplication : public sim::Application
{
public:
	virtual void run();
};

void MyApplication::run(){
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
