#include "strings.h"
#include "sim.h"
#include "log.h"

namespace sim{

const static int BUF_RESIZE_TRIGGER = 16 * 1024;

int Decoder::push(const char *buf, int len){
	buffer.append(buf, len);
	//log_debug("'%s'", str_escape(buffer).c_str());
	return len;
}

int Decoder::parse(Message *msg){
	msg->reset();

	if(buffer_offset >= BUF_RESIZE_TRIGGER){
		//log_debug("resize buffer");
		buffer = std::string(buffer.data() + buffer_offset, buffer.size() - buffer_offset);
		buffer_offset = 0;
	}
	
	while(buffer.size() > buffer_offset && isspace(buffer[buffer_offset])){
		buffer_offset ++;
	}
	if(buffer.size() == buffer_offset){
		return 0;
	}
	
	const char *key = buffer.data() + buffer_offset;
	const char *msg_end = (const char *)memchr(key, sim::MSG_END_BYTE, buffer.size() - buffer_offset);
	if(!msg_end){
		return 0;
	}
	int msg_len = msg_end - key + 1;
	int size = msg_len;
	
	int auto_tag = 0;
	while(1){
		int key_len = 0;
		int val_len;
		int tag;

		const char *end;
		end = (const char *)memchr(key, sim::KV_END_BYTE, size);
		// 兼容最后一个 空格 被省略的情况
		if(end == NULL){
			end = msg_end;
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
	
		val_len = end - val;
		size -= val_len + 1;

		if(val_len > 0 && val[val_len - 1] == '\r'){
			val_len -= 1;
		}

		//printf("%u key: %u, val: %u\n", __LINE__, key_len, val_len);

		std::string val_s(val, val_len);
		msg->set(tag, val_s);

		key = end + 1;
		auto_tag = tag + 1;
		
		if(key >= msg_end){
			std::map<int, std::string>::iterator it;
			for(it=msg->fields_.begin(); it!=msg->fields_.end(); it++){
				it->second = sim::decode(it->second);
			}
			buffer_offset += msg_len;
			//log_debug("msg.len: %d, buffer.len: %d", msg_len, buffer.size());
			return 1;
		}
	}
	return 0;
}

}; // namespace sim
