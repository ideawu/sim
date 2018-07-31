#include "line_parser.h"
#include <string>
#include "line_message.h"

ParseState LineParser::parse(Buffer *buffer, Message **msg){
	const char *data = buffer->data();
	char *end = (char *)memchr(data, '\n', buffer->size());
	if(!end){
		return ParseState::none_state();
	}
	int total = end - data + 1;
	int len = end - data;
	// 兼容 \r\n
	if(len >= 1 && data[len - 1] == '\r'){
		len -= 1;
	}
	
	std::string s(data, len);
	LineMessage *lm = new LineMessage();
	lm->text(s);
	*msg = lm;
	
	buffer->remove(total);
	
	return ParseState::ready_state();
}
