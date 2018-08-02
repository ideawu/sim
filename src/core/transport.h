#ifndef SIM_TRANSPORT_H
#define SIM_TRANSPORT_H

#include <vector>

class Event;
class Server;
class Message;

class Transport
{
public:
	static Transport* create();
	
	virtual ~Transport(){}
	
	virtual void add_server(Server *serv) = 0;
	virtual void init() = 0;
	
	virtual const std::vector<Event>* wait(int timeout_ms) = 0;
	
	// multi-thread safe
	virtual void accept(int id) = 0;
	// multi-thread safe
	virtual void close(int id) = 0;
	// multi-thread safe
	virtual Message* recv(int id) = 0;
	// multi-thread safe
	virtual void send(int id, Message *msg) = 0;
	
protected:
	Transport(){}
};

#endif
