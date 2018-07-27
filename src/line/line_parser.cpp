#include "line_parser.h"

ParseStatus LineParser::parse(Buffer *buffer, Message **msg){
	ParseStatus ret;
	
	const char *data = buffer->data();
	char *end = (char *)memchr(data, '\n', buffer->size());
	if(!end){
		return ret;
	}
	int len = end - data;
	// 兼容 \r\n
	if(len >= 1 && data[len - 1] == '\r'){
		len -= 1;
	}
	
	std::string s(data, len);
	LineMessage *lm = new LineMessage();
	lm->text(s);
	*msg = lm;
	
	ret.set_ok();
	return ret;
}
