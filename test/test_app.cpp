#include <stdio.h>
#include <stdlib.h>
#include "sim/sim.h"

class MyApplication : public sim::Application
{
public:
	virtual void loop_once();
	virtual int init();
	virtual void free();
};

int MyApplication::init(){
	log_info("server started.");
	return 0;
}

void MyApplication::free(){
	log_info("server exit.");
}

void MyApplication::loop_once(){
	sleep(1);
	log_debug("");
}

int main(int argc, char **argv){
	MyApplication app;
	return app.main(argc, argv);
}
