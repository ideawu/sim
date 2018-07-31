#include "line_server.h"
#include "line_parser.h"
#include "net/link.h"

LineServer::LineServer(){
}

LineServer::~LineServer(){
}

void LineServer::init(){
}

Session* LineServer::accept(){
	Link *link = this->link()->accept();
	if(!link){
		return NULL;
	}
	Session *sess = new Session(link, new LineParser());
	return sess;
}
