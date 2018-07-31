#ifndef SIM_PARSER_H
#define SIM_PARSER_H

class Buffer;
class Message;
class ParseState;

class Parser
{
public:
	virtual ~Parser(){};
	virtual ParseState parse(Buffer *buffer, Message **msg) = 0;
};

/////////////////////////////////////////

class ParseState
{
public:
	static ParseState none_state();
	static ParseState ready_state();
	static ParseState error_state();
	
	ParseState();
	~ParseState();
	
	bool none() const;
	bool ready() const;
	bool error() const;
	
private:
	ParseState(int code);

	int _code;
};

#endif
