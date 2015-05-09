#ifndef SIM_SERVER_H_
#define SIM_SERVER_H_

#include <string>
#include <vector>
#include <map>
#include "link.h"
#include "handler.h"

namespace sim{

class Fdevents;

class Server
{
public:
	static Server* listen(const std::string &ip, int port);
	//static Server* create(...);
	Server();
	~Server();
	
	void add_handler(Handler *handler);
	void loop();
	int loop_once();
	
	//int send(int64_t sess_id, const Message &msg);
	//int send_all(const Message &msg);
private:
	Fdevents *fdes;
	Link *serv_link;
	int link_count;
	std::map<int64_t, Session *> sessions;
	std::vector<Handler *> handlers;

	Session* accept_session();
	Session* get_session(int64_t sess_id);
	int close_session(Session *sess);
	int read_session(Session *sess);
	int write_session(Session *sess);
};

}; // namespace sim

#endif
