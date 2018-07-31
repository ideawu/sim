#include "event.h"

Event Event::new_event(Session *sess){
	return Event(sess->id(), 0);
}
Event Event::close_event(Session *sess){
	return Event(sess->id(), 1);
}
Event Event::read_event(Session *sess){
	return Event(sess->id(), 2);
}

Event::Event(){
	_type = -1;
}

Event::Event(int id, int type){
	_id = id;
	_type = type;
}

int Event::id() const{
	return _id;
}

bool Event::is_none() const{
	return _type == -1;
}

bool Event::is_new() const{
	return _type == 0;
}

bool Event::is_close() const{
	return _type == 1;
}

bool Event::is_read() const{
	return _type == 2;
}
