#ifndef SIM_MESSAGE_H
#define SIM_MESSAGE_H

#include "buffer.h"

class Message
{
public:
	virtual Buffer* encode() = 0;
};

#endif
