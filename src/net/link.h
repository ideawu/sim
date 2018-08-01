#ifndef SIM_LINK_H
#define SIM_LINK_H

#include <arpa/inet.h>
#include <string>

class Buffer;

class Link
{
public:
	Link();
	virtual ~Link();

	int fd() const;
	Buffer* input() const;
	Buffer* output() const;
	std::string address() const;
	
	bool nonblock() const;
	void nonblock(bool enable);

	void close();
	virtual Link* accept() = 0;
	virtual int net_read() = 0;
	virtual int net_write() = 0;

protected:
	int _fd;
	Buffer* _input;
	Buffer* _output;
	std::string _address;
	
private:
	bool _nonblock;
};

#endif
