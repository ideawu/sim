#ifndef SIM_LINK_H
#define SIM_LINK_H

#include <arpa/inet.h>
#include <string>

class Link
{
public:
	Link();
	virtual ~Link();

	int fd() const;
	std::string address() const;
	
	bool nonblock() const;
	void nonblock(bool enable);

	void close();
	virtual int net_read() = 0;

protected:
	int _fd;
	std::string _address;
	
private:
	bool _nonblock;
};

#endif
