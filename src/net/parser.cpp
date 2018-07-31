#include "parser.h"

ParseStatus::ParseStatus(){
	_code = 0;
}

ParseStatus::~ParseStatus(){
}

bool ParseStatus::ok() const{
	return _code == 1;
}

bool ParseStatus::none() const{
	return _code == 0;
}

bool ParseStatus::error() const{
	return _code == -1;
}


void ParseStatus::set_ok(){
	_code = 1;
}

void ParseStatus::set_none(){
	_code = 0;
}

void ParseStatus::set_error(){
	_code = -1;
}

