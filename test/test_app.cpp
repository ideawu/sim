#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"

class MyApplication : public sim::Application
{
public:
	virtual int loop_once();
	virtual int init();
	virtual int free();
};

int MyApplication::init(){
	log_info("server started.");
	return 0;
}

int MyApplication::free(){
	log_info("server exit.");
	return 0;
}

int MyApplication::loop_once(){
	sleep(1);
	log_debug("");
	return 0;
}

int main(int argc, char **argv){
	MyApplication app;
	return app.main(argc, argv);
}
