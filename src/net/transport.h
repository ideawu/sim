#ifndef SIM_TRANSPORT_H
#define SIM_TRANSPORT_H

#include <map>
#include "util/thread.h"
#include "fde.h"
#include "event.h"
#include "session.h"

using namespace sim;

class Transport
{
public:
	Transport();
	~Transport();
	
	Event wait(int timeout_ms);
	
	void accept(int id);
	void close(int id);
	
	Message* recv(int id);
	// void send(int id, Message *msg);
	
	void setup();
	
private:
	Mutex _mutex;
	
	std::map<int, Session*> _working_list;
	std::map<int, Session*> _opening_list;
	std::map<int, Session*> _closing_list;
	
	Queue<Event> _events;
	
	Channel<int> _accept_ids;
	Channel<int> _close_ids;
	
	void handle_on_new(Session *sess);
	void handle_on_close(Session *sess);
	void handle_on_read(Session *sess);
	void handle_accept_id();
	void handle_close_id();

	Fdevents *_fdes;
	
	static void* run(void *arg);
};

#endif
