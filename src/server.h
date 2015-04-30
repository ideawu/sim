#ifndef SIM_SERVER_H_
#define SIM_SERVER_H_

#include <string>
#include <vector>
#include <map>
#include "link.h"
#include "handler.h"

class Fdevents;

namespace sim{

class Server
{
public:
	static Server* listen(const std::string &ip, int port);
	//static Server* create(...);
	Server();
	~Server();
	
	void loop();
	void add_handler(Handler *handler);

private:
	Fdevents *fdes;
	Link *serv_link;
	int link_count;
	std::vector<Handler*> handlers;

	Link* accept_link();
	int close_link(Link *link);
	int read_link(Link *link);
	int write_link(Link *link);
};

}; // namespace sim

#endif
