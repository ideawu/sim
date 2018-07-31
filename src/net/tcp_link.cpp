#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include "util/log.h"
#include "tcp_link.h"

// namespace sim{

TcpLink::TcpLink(bool is_server){
	remote_ip[0] = '\0';
	remote_port = -1;
	_buffer = new Buffer();
}

TcpLink::~TcpLink(){
	delete _buffer;
}

Buffer* TcpLink::buffer() const{
	return _buffer;
}

void TcpLink::nodelay(bool enable){
	int opt = enable? 1 : 0;
	::setsockopt(fd(), IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
}

void TcpLink::keepalive(bool enable){
	int opt = enable? 1 : 0;
	::setsockopt(fd(), SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(opt));
}

TcpLink* TcpLink::connect(const std::string &ip, int port){
	return connect(ip.c_str(), port);
}

TcpLink* TcpLink::connect(const char *ip, int port){
	TcpLink *link;
	int sock = -1;

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);
	inet_pton(AF_INET, ip, &addr.sin_addr);

	if((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1){
		goto sock_err;
	}
	if(::connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		goto sock_err;
	}

	//log_debug("fd: %d, connect to %s:%d", sock, ip, port);
	link = new TcpLink();
	link->_fd = sock;
	link->keepalive(true);
	return link;
sock_err:
	//log_debug("connect to %s:%d failed: %s", ip, port, strerror(errno));
	if(sock >= 0){
		::close(sock);
	}
	return NULL;
}

TcpLink* TcpLink::listen(const std::string &ip, int port){
	return listen(ip.c_str(), port);
}

TcpLink* TcpLink::listen(const char *ip, int port){
	TcpLink *link;
	int sock = -1;

	int opt = 1;
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);
	inet_pton(AF_INET, ip, &addr.sin_addr);

	if((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1){
		goto sock_err;
	}
	if(::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		goto sock_err;
	}
	if(::bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		goto sock_err;
	}
	if(::listen(sock, 1024) == -1){
		goto sock_err;
	}
	//log_debug("server socket fd: %d, listen on: %s:%d", sock, ip, port);

	link = new TcpLink(true);
	link->_fd = sock;
	snprintf(link->remote_ip, sizeof(link->remote_ip), "%s", ip);
	link->remote_port = port;
	return link;
sock_err:
	//log_debug("listen %s:%d failed: %s", ip, port, strerror(errno));
	if(sock >= 0){
		::close(sock);
	}
	return NULL;
}

TcpLink* TcpLink::accept(){
	TcpLink *link;
	int client_sock;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	while((client_sock = ::accept(fd(), (struct sockaddr *)&addr, &addrlen)) == -1){
		if(errno != EINTR){
			//log_error("socket %d accept failed: %s", sock, strerror(errno));
			return NULL;
		}
	}

	struct linger opt = {1, 0};
	int ret = ::setsockopt(client_sock, SOL_SOCKET, SO_LINGER, (void *)&opt, sizeof(opt));
	if (ret != 0) {
		//log_error("socket %d set linger failed: %s", client_sock, strerror(errno));
	}

	link = new TcpLink();
	link->_fd = client_sock;
	link->keepalive(true);
	inet_ntop(AF_INET, &addr.sin_addr, link->remote_ip, sizeof(link->remote_ip));
	link->remote_port = ntohs(addr.sin_port);
	return link;
}

int TcpLink::net_read(){
	int ret = 0;
	char buf[8 * 1024];
	int want = sizeof(buf);
	while(1){
		// test
		//want = 1;
		int len = ::read(fd(), buf, want);
		if(len == -1){
			if(errno == EINTR){
				continue;
			}else if(errno == EWOULDBLOCK){
				break;
			}else{
				//log_debug("fd: %d, read: -1, want: %d, error: %s", sock, want, strerror(errno));
				return -1;
			}
		}else{
			//log_debug("fd: %d, want=%d, read: %d", sock, want, len);
			if(len == 0){
				return 0;
			}
			ret += len;
			
			_buffer->append(buf, len);
		}
		if(!nonblock()){
			break;
		}
	}
	//log_debug("read %d", ret);
	return ret;
}

// }; // namespace sim
