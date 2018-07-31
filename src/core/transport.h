#ifndef SIM_TRANSPORT_H
#define SIM_TRANSPORT_H

class Event;
class Server;
class Message;

class Transport
{
public:
	static Transport* create();
	
	virtual ~Transport(){}
	
	virtual void add_server(Server *serv) = 0;
	virtual void setup() = 0;
	
	virtual Event wait(int timeout_ms) = 0;
	
	virtual void accept(int id) = 0;
	virtual void close(int id) = 0;
	
	virtual Message* recv(int id) = 0;
	// void send(int id, Message *msg);
	
protected:
	Transport(){}
};

#endif
