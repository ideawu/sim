#include "line_message.h"

std::string LineMessage::text() const{
	return _text;
}

void LineMessage::text(const std::string &s){
	_text = s;
}

Buffer* LineMessage::encode(){
	Buffer *buf = new Buffer();
	buf->append(_text.data(), _text.size());
	buf->append("\n", 1);
	return buf;
}
