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

class TcpSocket{
public:
	char remote_ip[INET_ADDRSTRLEN];
	int remote_port;

	double create_time;
	double active_time;

	~TcpSocket();
	void close();
	void nodelay(bool enable=true);
	void noblock(bool enable=true);
	void keepalive(bool enable=true);

	int fd() const;
	Buffer* buffer() const;

	int net_read();
	// int net_write();

	static TcpSocket* connect(const char *ip, int port);
	static TcpSocket* connect(const std::string &ip, int port);
	static TcpSocket* listen(const char *ip, int port);
	static TcpSocket* listen(const std::string &ip, int port);
	TcpSocket* accept();

private:
	int sock;
	bool noblock_;
	Buffer* _buffer;
	
	TcpSocket(bool is_server=false);

};


// }; // namespace sim

#endif
