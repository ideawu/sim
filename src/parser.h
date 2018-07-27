#ifndef SIM_PARSER_H
#define SIM_PARSER_H

#include "buffer.h"
#include "message.h"

class ParseStatus;

class Parser
{
public:
	virtual ParseStatus parse(Buffer *buffer, Message **msg) = 0;
};

/////////////////////////////////////////

class ParseStatus
{
public:
	ParseStatus();
	~ParseStatus();
	
	bool ok() const;
	bool none() const;
	bool error() const;
	
	void set_ok();
	void set_none();
	void set_error();
	
private:
	int _code;
};

#endif
