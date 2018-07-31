#ifndef SIM_LINE_SERVER_H
#define SIM_LINE_SERVER_H

#include "server.h"

class LineServer : public Server
{
public:
	LineServer();
	~LineServer();
	
	virtual void init();
	virtual Session* accept();
};

#endif
