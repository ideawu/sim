#ifndef SIM_SIM_H_
#define SIM_SIM_H_

#include "log.h"
#include "thread.h"
#include "message.h"
#include "decoder.h"
#include "link.h"
#include "server.h"
#include "handler.h"

namespace sim{

const static char KV_SEP_BYTE = '=';
const static char KV_END_BYTE = ' ';
const static char MSG_END_BYTE = '\n';

std::string encode(const std::string s);
std::string decode(const std::string s);

}; // namespace sim

#endif
