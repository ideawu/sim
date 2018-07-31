#ifndef SIM_SESSION_H
#define SIM_SESSION_H

#include <list>
#include "link.h"
#include "parser.h"
#include "message.h"

class Session
{
public:
	Session(Link *link, Parser *parser);
	~Session();
	
	int id() const;
	Link* link() const;
	Parser* parser() const;
	
	ParseState parse();
	
	Message* recv();
	
private:
	int _id;
	Link *_link;
	Parser *_parser;

	std::list<Message *> _input;
	std::list<Message *> _output;
};

#endif
