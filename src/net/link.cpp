#include "link.h"
#include <unistd.h>
#include <fcntl.h>
#include "util/buffer.h"

Link::Link(){
	_fd = -1;
	_nonblock = false;
	_buffer = new Buffer();
}

Link::~Link(){
	this->close();
	delete _buffer;
}

int Link::fd() const{
	return _fd;
}

Buffer* Link::buffer() const{
	return _buffer;
}

std::string Link::address() const{
	return _address;
}

void Link::close(){
	if(_fd >= 0){
		::close(_fd);
		_fd = -1;
	}
}

bool Link::nonblock() const{
	return _nonblock;
}

void Link::nonblock(bool enable){
	_nonblock = enable;
	if(enable){
		::fcntl(_fd, F_SETFL, O_NONBLOCK | O_RDWR);
	}else{
		::fcntl(_fd, F_SETFL, O_RDWR);
	}
}
