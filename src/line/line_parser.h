#ifndef SIM_LINE_PARSER_H
#define SIM_LINE_PARSER_H

#include "net/parser.h"
#include "net/message.h"
#include "net/session.h"
#include "util/buffer.h"

class LineParser : public Parser
{
public:
	virtual ParseState parse(Buffer *buffer, Message **msg);
};

#endif
