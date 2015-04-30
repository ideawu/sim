#ifndef SIM_SERVER_H_
#define SIM_SERVER_H_

#include <string>
#include <map>
#include "link.h"
#include "fde.h"

namespace sim{

class Server
{
public:
	static Server* listen(const std::string &ip, int port);
	Server();
	~Server();

private:
	Fdevents *fdes;
	Link *serv;
	std::map<int, Link*> links;
};

}; // namespace sim

#endif
