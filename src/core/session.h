#ifndef SIM_SESSION_H
#define SIM_SESSION_H

#include <list>

class Link;
class Parser;
class Message;

class Session
{
public:
	Session(Link *link, Parser *parser);
	virtual ~Session();
	
	int id() const;
	Link* link() const;
	Parser* parser() const;
	
	const std::list<Message *>* input() const;
	const std::list<Message *>* output() const;
	
	// 返回解析成功的要接收的报文的数量，出错返回-1
	virtual int parse_input();
	// 返回编码成功的要发送的报文的数量，出错返回-1
	virtual int encode_output();

	Message* recv();
	void send(Message *msg);
	
private:
	int _id;
	Link *_link;
	Parser *_parser;

	std::list<Message *> _input;
	std::list<Message *> _output;
};

#endif
