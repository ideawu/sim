#ifndef SIM_LINE_SERVER_H
#define SIM_LINE_SERVER_H

#include "net/server.h"
#include "net/session.h"

class LineServer : public Server
{
public:
	LineServer();
	~LineServer();
	
	virtual void init();
	virtual Session* accept();
};

#endif
