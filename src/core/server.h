#ifndef SIM_SERVER_H
#define SIM_SERVER_H

#include "session.h"

class Link;

class Server
{
public:
	Server();
	virtual ~Server();

	Link* link() const;
	
	virtual void init() = 0;
	// 接受新连接
	virtual Session* accept() = 0;

	int listen(const char *host, int port);
	
private:
	Link *_link;
};

#endif
