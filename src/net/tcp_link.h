#ifndef SIM_TCP_SOCKET_H_
#define SIM_TCP_SOCKET_H_

#include <string>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "util/buffer.h"
#include "link.h"

// namespace sim{

class TcpLink : public Link{
public:
	virtual ~TcpLink();
	void nodelay(bool enable=true);
	void keepalive(bool enable=true);

	Buffer* buffer() const;

	int net_read();
	// int net_write();

	static TcpLink* connect(const char *ip, int port);
	static TcpLink* connect(const std::string &ip, int port);
	static TcpLink* listen(const char *ip, int port);
	static TcpLink* listen(const std::string &ip, int port);
	TcpLink* accept();

private:
	Buffer* _buffer;
	
	TcpLink(bool is_server=false);
};


// }; // namespace sim

#endif
