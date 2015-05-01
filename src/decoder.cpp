#include "strings.h"
#include "sim.h"
#include "log.h"

namespace sim{

int Decoder::push(const char *buf, int len){
	buffer_.append(buf, len);
	//log_debug("%s", buffer_.c_str());
	return len;
}

int Decoder::parse(Message *msg){
	msg->reset();
	if(buffer_.empty()){
		return 0;
	}
	
	const char *key = buffer_.data();
	for(int i=0; i<(int)buffer_.size(); i++){
		if(isspace(buffer_[i])){
			key ++;
		}else{
			break;
		}
	}
	const char *msg_end = (const char *)memchr(key, sim::MSG_END_BYTE, buffer_.size());
	if(!msg_end){
		return 0;
	}
	int msg_len = msg_end - buffer_.data() + 1;
	int size = msg_len;
	
	int auto_tag = 0;
	while(1){
		int key_len;
		int val_len;
		int tag;

		const char *end;
		end = (const char *)memchr(key, sim::KV_END_BYTE, size);
		// 兼容最后一个 \s 被省略的情况
		if(end == NULL){
			end = msg_end - 1;
		}

		const char *val = (const char *)memchr(key, sim::KV_SEP_BYTE, end - key);
		if(val == NULL){
			val = key;
			tag = auto_tag;
		}else{
			val ++;
			key_len = val - key - 1;
			size -= key_len + 1;
			std::string key_s(key, key_len);
			tag = str_to_int(key_s);
		}

		//printf("%u key: %u, val: %u, end: %u, %u\n", __LINE__, key, val, end, end - key);
	
		val_len = end - val;
		size -= val_len + 1;

		if(val_len > 0 && val[val_len - 1] == '\r'){
			val_len -= 1;
		}
		std::string val_s(val, val_len);
		msg->set(tag, val_s);

		key = end + 1;
		auto_tag = tag + 1;
		
		if(key == msg_end){
			std::map<int, std::string>::iterator it;
			for(it=msg->fields_.begin(); it!=msg->fields_.end(); it++){
				it->second = sim::decode(it->second);
			}
			// 一个完整的报文解析结束, 从缓冲区清除已经解析了的数据
			buffer_ = std::string(key+1, buffer_.size() - msg_len); // TODO: 可以优化
			//log_debug("msg.len: %d, buffer.len: %d", msg_len, buffer_.size());
			return 1;
		}
	}
	return 0;
}

}; // namespace sim
