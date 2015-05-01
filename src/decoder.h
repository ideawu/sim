#ifndef SIM_DECODER_H_
#define SIM_DECODER_H_

#include <string>
#include "message.h"

namespace sim{

class Decoder{
public:
	Decoder(){
		buffer_offset = 0;
	}
	// 当你从 socket 中读到数据, 或者从文件中读到数据时, 将数据压入解码器的数据缓冲区
	int push(const char *buf, int len);
	// 解析缓冲区中的数据, 如果解析出一个完整的 FIX 报文, 返回 1,
	// 如果数据不足一个报文, 返回 0, 你应该继续读取数据并压入解码器.
	// 如果出错(如系统错误), 返回 -1.
	int parse(Message *msg);
private:
	std::string buffer_;
	int buffer_offset;
};

}; // namespace sim

#endif
