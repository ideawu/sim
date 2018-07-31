#include "session.h"

static int id_incr = 1;

Session::Session(Link *link){
	_id = id_incr++;
	_link = link;
}

Session::~Session(){
}

int Session::id() const{
	return _id;
}

Link* Session::link() const{
	return _link;
}
