#ifndef SIM_LINE_PARSER_H
#define SIM_LINE_PARSER_H

#include "parser.h"

class LineParser : public Parser
{
public:
	virtual ParseState parse(Buffer *buffer, Message **msg);
};

#endif
