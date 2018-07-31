#ifndef SIM_LINE_SERVER_H
#define SIM_LINE_SERVER_H

#include "core/server.h"
#include "core/session.h"

class LineServer : public Server
{
public:
	LineServer();
	~LineServer();
	
	virtual void init();
	virtual Session* accept();
};

#endif
