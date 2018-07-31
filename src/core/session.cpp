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

int Session::parse(){
	int ret = 0;
	while(1){
		Message *msg;
		ParseState s = _parser->parse(_link->buffer(), &msg);
		if(s.ready()){
			_input.push_back(msg);
			ret ++;
		}else if(s.error()){
			return -1;
		}else{
			break;
		}
	}
	log_debug("received %d message(s)", ret);
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
