#ifndef SIM_EVENT_H
#define SIM_EVENT_H

#include "session.h"

class Event
{
public:
	static Event new_event(Session *sess);
	static Event close_event(Session *sess);
	static Event read_event(Session *sess);
	
	Event();
	Event(int id, int type);
	int id() const;
	bool is_none() const;
	bool is_new() const;
	bool is_close() const;
	bool is_read() const; // 收到一个报文

private:
	int _id;
	int _type;
};

#endif
