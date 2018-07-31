#ifndef SIM_MESSAGE_H
#define SIM_MESSAGE_H

#include "util/buffer.h"

class Message
{
public:
	virtual ~Message(){}
	virtual Buffer* encode() = 0;
};

#endif
