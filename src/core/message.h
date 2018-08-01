#ifndef SIM_MESSAGE_H
#define SIM_MESSAGE_H

#include "util/buffer.h"

class Message
{
public:
	virtual ~Message(){}
	virtual int encode(Buffer *buffer) = 0;
};

#endif
