#ifndef SIM_LINK_H
#define SIM_LINK_H

#include <arpa/inet.h>

class Link
{
public:
	// TODO:
	char remote_ip[INET_ADDRSTRLEN];
	int remote_port;

	Link();
	virtual ~Link();

	int id() const;
	int fd() const;
	void close();
	
	bool nonblock() const;
	void nonblock(bool enable);

protected:
	int _fd;
	
private:
	int _id;
	bool _nonblock;
};

#endif
