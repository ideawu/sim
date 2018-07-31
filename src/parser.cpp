#include "parser.h"

ParseState ParseState::none_state(){
	return ParseState(0);
}

ParseState ParseState::ready_state(){
	return ParseState(1);
}

ParseState ParseState::error_state(){
	return ParseState(-1);
}

ParseState::ParseState(){
	_code = 0;
}

ParseState::ParseState(int code){
	_code = code;
}

ParseState::~ParseState(){
}

bool ParseState::none() const{
	return _code == 0;
}

bool ParseState::ready() const{
	return _code == 1;
}

bool ParseState::error() const{
	return _code == -1;
}

