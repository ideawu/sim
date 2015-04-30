#ifndef FIX_FIX_H_
#define FIX_FIX_H_

#include "message.h"
#include "decoder.h"

namespace sim{

const static char KV_SEP_BYTE = '=';
const static char KV_END_BYTE = ' ';
const static char MSG_END_BYTE = '\n';

std::string encode(const std::string s);
std::string decode(const std::string s);

}; // namespace sim

#endif
