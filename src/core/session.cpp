#include "session.h"
#include "util/log.h"
#include "net/link.h"
#include "parser.h"
#include "message.h"

static int id_incr = 1;

Session::Session(Link *link, Parser *parser){
	_id = id_incr++;
	_link = link;
	_parser = parser;
}

Session::~Session(){
	delete _link;
	delete _parser;
	for(std::list<Message *>::iterator it=_input.begin(); it!=_input.end(); it++){
		Message *msg = *it;
		delete msg;
	}
	for(std::list<Message *>::iterator it=_output.begin(); it!=_output.end(); it++){
		Message *msg = *it;
		delete msg;
	}
}

int Session::id() const{
	return _id;
}

Link* Session::link() const{
	return _link;
}

Parser* Session::parser() const{
	return _parser;
}

const std::list<Message *>* Session::input() const{
	return &_input;
}

const std::list<Message *>* Session::output() const{
	return &_output;
}

int Session::parse_input(){
	int ret = 0;
	while(1){
		Message *msg;
		ParseState s = _parser->parse(_link->input(), &msg);
		if(s.ready()){
			_input.push_back(msg);
			ret ++;
		}else if(s.error()){
			return -1;
		}else{
			break;
		}
	}
	return ret;
}

int Session::encode_output(){
	int ret = 0;
	while(!_output.empty()){
		Message *msg = _output.front();
		_output.pop_front();
		msg->encode(_link->output());
		delete msg;
		ret ++;
	}
	return ret;
}

Message* Session::recv(){
	if(_input.empty()){
		return NULL;
	}
	Message *msg = _input.front();
	_input.pop_front();
	return msg;
}

void Session::send(Message *msg){
	_output.push_back(msg);
}
