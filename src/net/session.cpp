#include "session.h"
#include "util/log.h"

static int id_incr = 1;

Session::Session(Link *link, Parser *parser){
	_id = id_incr++;
	_link = link;
	_parser = parser;
}

Session::~Session(){
	delete _link;
	delete _parser;
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

ParseState Session::parse(){
	while(1){
		Message *msg;
		ParseState s = _parser->parse(_link->buffer(), &msg);
		if(s.ready()){
			_input.push_back(msg);
		}else if(s.error()){
			_input.clear();
			_output.clear();
			return s;
		}else{
			break;
		}
	}
	
	log_debug("received %d message(s)", (int)_input.size());
	if(_input.empty()){
		return ParseState::none_state();
	}else{
		return ParseState::ready_state();
	}
}
