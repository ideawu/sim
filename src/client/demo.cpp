#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <vector>
#include "client.h"

int main(int argc, char **argv){
	printf("Usage: %s [ip] [port]\n", argv[0]);
	const char *ip = (argc >= 2)? argv[1] : "127.0.0.1";
	int port = (argc >= 3)? atoi(argv[2]) : 8800;

	// connect to server
	sim::Client *client = sim::Client::connect(ip, port);
	if(client == NULL){
		printf("fail to connect to server!\n");
		return 0;
	}
	
	sim::Message req, resp;
	req.set_type("ping");
	client->send(req);
	client->recv(&resp);
	printf("resp: %s\n", resp.encode().c_str());
	
	delete client;
	return 0;
}
