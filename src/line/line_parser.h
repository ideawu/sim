#ifndef SIM_LINE_PARSER_H
#define SIM_LINE_PARSER_H

#include "core/parser.h"
#include "core/message.h"
#include "core/session.h"
#include "util/buffer.h"

class LineParser : public Parser
{
public:
	virtual ParseState parse(Buffer *buffer, Message **msg);
};

#endif
