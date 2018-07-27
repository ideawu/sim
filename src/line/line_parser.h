#ifndef SIM_LINE_PARSER_H
#define SIM_LINE_PARSER_H

#include <string>
#include "parser.h"
#include "line_message.h"

class LineParser : public Parser
{
public:
	virtual ParseStatus parse(Buffer *buffer, Message **msg);
};

#endif
