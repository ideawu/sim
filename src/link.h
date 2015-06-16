#ifndef SIM_LINK_H_
#define SIM_LINK_H_

#include <string>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "decoder.h"

namespace sim{

class Link{
private:
	int sock;
	bool noblock_;
	bool error_;
	Decoder decoder_;
	Link(bool is_server=false);
	
	// TODO: max_recv_buf_size, max_send_buf_size
public:
	std::string output;

	char remote_ip[INET_ADDRSTRLEN];
	int remote_port;

	double create_time;
	double active_time;

	~Link();
	void close();
	void nodelay(bool enable=true);
	// noblock(true) is supposed to corperate with IO Multiplex,
	// otherwise, flush() may cause a lot unneccessary write calls.
	void noblock(bool enable=true);
	void keepalive(bool enable=true);

	int fd() const{
		return sock;
	}
	bool error() const{
		return error_;
	}
	void mark_error(){
		error_ = true;
	}

	static Link* connect(const char *ip, int port);
	static Link* connect(const std::string &ip, int port);
	static Link* listen(const char *ip, int port);
	static Link* listen(const std::string &ip, int port);
	Link* accept();

	// read network data info buffer
	int read();
	int write();
	// flush buffered data to network
	// REQUIRES: nonblock
	int flush();

	int recv(Message *msg);
	int send(const Message &msg);
};


}; // namespace sim

#endif
