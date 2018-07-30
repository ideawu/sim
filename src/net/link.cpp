#include "link.h"
#include <unistd.h>

Link::Link(){
	_status = 0;
	_fd = -1;
}

Link::~Link(){
}

int Link::fd() const{
	return _fd;
}
	
bool Link::is_new() const{
	return _status == 0;
}

bool Link::is_closed() const{
	return _status == 1;
}

bool Link::is_working() const{
	return _status == 2;
}

void Link::accept(){
	_status = 2;
}

void Link::close(){
	if(is_closed()){
		return;
	}
	_status = 1;
	::close(_fd);
	_fd = -1;
}
