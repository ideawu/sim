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
	virtual ~Session();
	
	int id() const;
	Link* link() const;
	Parser* parser() const;
	
	// 返回解析成功的报文数量，出错返回-1
	virtual int parse();

	Message* recv();
	
private:
	int _id;
	Link *_link;
	Parser *_parser;

	std::list<Message *> _input;
	std::list<Message *> _output;
};

#endif
