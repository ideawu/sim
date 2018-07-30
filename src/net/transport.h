#ifndef SIM_TRANSPORT_H
#define SIM_TRANSPORT_H

#include <set>
#include <list>
#include "util/thread.h"
#include "link.h"

class Transport
{
public:
	Transport();
	~Transport();
	
	Link* listen();
	void accept(Link *link);
	void close(Link *link);
	
	void setup();
	
private:
	Mutex _mutex;
	Semaphore _signal;
	std::list<Link*> _waiting_links;
	std::set<Link*> _working_links;
	
	void* run(void *arg);
};

#endif
