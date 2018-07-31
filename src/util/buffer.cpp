#include "buffer.h"
#include <stdlib.h>
#include <string.h>

Buffer::Buffer(){
	_size = 0;
	_capacity = 1024;
	_buf = (char *)malloc(_capacity);
}

Buffer::~Buffer(){
	free(_buf);
}

int Buffer::size() const{
	return _size;
}

char* Buffer::data(){
	return _buf;
}

int Buffer::remove(int count){
	memmove(_buf, _buf + count, _size - count);
	_size -= count;
	return count;
}

int Buffer::append(const char *buf, int count){
	if(_size + count > _capacity){
		_capacity = _size + count;
		_buf = (char *)realloc(_buf, _capacity);
	}
	memcpy(_buf + _size, buf, count);
	_size += count;
	return count;
}
