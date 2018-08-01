#ifndef SIM_TRANSPORT_IMPL_H
#define SIM_TRANSPORT_IMPL_H

#include <map>
#include <vector>
#include "util/thread.h"
#include "transport.h"
#include "event.h"
#include "server.h"
#include "session.h"

namespace sim{
	class Fdevents;
};
using namespace sim;

class TransportImpl : public Transport
{
public:
	TransportImpl();
	~TransportImpl();
	
	virtual void add_server(Server *serv);
	virtual void setup();
	
	virtual Event wait(int timeout_ms);
	
	virtual void accept(int id);
	virtual void close(int id);
	
	virtual Message* recv(int id);
	virtual void send(int id, Message *msg);
	
private:
	Mutex _mutex;
	
	std::map<int, Session*> _working_list;
	std::map<int, Session*> _opening_list;
	std::map<int, Session*> _closing_list;
	
	Queue<Event> _events;
	
	Channel<int> _accept_ids;
	Channel<int> _close_ids;
	Channel<int> _send_ids;
	
	void handle_on_new(Session *sess);
	void handle_on_close(Session *sess);
	void handle_on_read(Session *sess);
	void handle_on_write(Session *sess);
	void handle_accept_id();
	void handle_close_id();
	void handle_send_id();

	Fdevents *_fdes;
	
	std::vector<Server*> _servers;
	
	static void* run(void *arg);
};

#endif
