#ifndef SIM_TCP_SOCKET_H_
#define SIM_TCP_SOCKET_H_

#include <string>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "util/buffer.h"

// namespace sim{

class TcpLink{
public:
	char remote_ip[INET_ADDRSTRLEN];
	int remote_port;

	double create_time;
	double active_time;

	~TcpLink();
	void close();
	void nodelay(bool enable=true);
	void noblock(bool enable=true);
	void keepalive(bool enable=true);

	int id() const;
	int fd() const;
	Buffer* buffer() const;

	int net_read();
	// int net_write();

	static TcpLink* connect(const char *ip, int port);
	static TcpLink* connect(const std::string &ip, int port);
	static TcpLink* listen(const char *ip, int port);
	static TcpLink* listen(const std::string &ip, int port);
	TcpLink* accept();

private:
	int _id;
	int sock;
	bool noblock_;
	int _status;
	Buffer* _buffer;
	
	TcpLink(bool is_server=false);
};


// }; // namespace sim

#endif
