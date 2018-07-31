#ifndef SIM_TRANSPORT_H
#define SIM_TRANSPORT_H

#include <map>
#include "util/thread.h"
#include "tcp_link.h"
#include "fde.h"

using namespace sim;


class LinkEvent
{
public:
	LinkEvent(){
		_status = -1;
	}
	LinkEvent(int id, int status){
		_id = id;
		_status = status;
	}
	
	int id() const{
		return _id;
	}
	bool is_none() const{
		return _status == -1;
	}
	bool is_new() const{
		return _status == 0;
	}
	bool is_close() const{
		return _status == 1;
	}
	bool is_read() const{
		return _status == 2;
	}
	static LinkEvent new_link(Link *link){
		return LinkEvent(link->id(), 0);
	}
	static LinkEvent close_link(Link *link){
		return LinkEvent(link->id(), 1);
	}
	static LinkEvent read_link(Link *link){
		return LinkEvent(link->id(), 2);
	}
	
	friend class Transport;
private:
	int _id;
	int _status;
};

class Transport
{
public:
	Transport();
	~Transport();
	
	LinkEvent wait(int timeout_ms);
	
	void accept(int id);
	void close(int id);
	
	// Message* recv(int id);
	// void send(int id, Message *msg);
	
	void setup();
	
private:
	Mutex _mutex;
	
	std::map<int, Link*> _working_list;
	std::map<int, Link*> _opening_list;
	std::map<int, Link*> _closing_list;
	
	Queue<LinkEvent> _events;
	
	Channel<int> _accept_ids;
	Channel<int> _close_ids;
	
	void handle_on_new(Link *link);
	void handle_on_close(Link *link);
	void handle_accept_id();
	void handle_close_id();

	Fdevents *_fdes;
	
	static void* run(void *arg);
};

#endif
