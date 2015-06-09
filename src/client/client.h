#ifndef SIM_CLIENT_H_
#define SIM_CLIENT_H_

#include "message.h"

namespace sim{

class Link;
std::string encode(const std::string s, bool force_ascii=false);
std::string decode(const std::string s);

class Client
{
public:
	static Client* connect(const char *ip, int port);
	static Client* connect(const std::string &ip, int port);

	int send(const Message &msg); // blocking send
	int recv(Message *msg); // blocking recv

	~Client();
private:
	Client();
	Link *link;
};

}; // namespace sim

#endif
