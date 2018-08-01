#include "line_message.h"

std::string LineMessage::text() const{
	return _text;
}

void LineMessage::text(const std::string &s){
	_text = s;
}

int LineMessage::encode(Buffer *buffer){
	buffer->append(_text.data(), _text.size());
	buffer->append("\n", 1);
	return _text.size() + 1;
}
