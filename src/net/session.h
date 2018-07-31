#ifndef SIM_SESSION_H
#define SIM_SESSION_H

#include "link.h"

class Session
{
public:
	Session(Link *link);
	~Session();
	
	int id() const;
	Link* link() const;

private:
	int _id;
	Link *_link;
};

#endif
