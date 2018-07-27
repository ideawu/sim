#include "buffer.h"

Buffer::Buffer(){
	_size = 0;
	_capacity = 1024;
	_buf = malloc(_capacity);
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
	memmove(_buf + count, _buf, _size - count);
	_size -= count;
	return count;
}

int Buffer::append(char *buf, int count){
	if(_size + count > _capacity){
		_capacity = _size + count;
		_buf = realloc(_buf, _capacity);
	}
	memcpy(_buf + _size, buf, count);
	_size += count;
	return count;
}
