#include "strings.h"
#include "sim.h"
#include "log.h"

namespace sim{

const static int BUF_RESIZE_TRIGGER = 16 * 1024;

int Decoder::push(const char *buf, int len){
	buffer_.append(buf, len);
	//log_debug("'%s'", str_escape(buffer_).c_str());
	return len;
}

int Decoder::parse(Message *msg){
	msg->reset();

	if(buffer_offset >= BUF_RESIZE_TRIGGER){
		//log_debug("resize buffer");
		buffer_ = std::string(buffer_.data() + buffer_offset, buffer_.size() - buffer_offset);
		buffer_offset = 0;
	}
	
	int data_size = buffer_.size() - buffer_offset;
	if(data_size == 0){
		return 0;
	}
	
	const char *data = buffer_.data() + buffer_offset;
	const char *key = data;
	for(int i=0; i<data_size && isspace(data[i]); i++){
		key ++;
	}
	buffer_offset += (key - data);
	data_size -= (key - data);
	
	const char *msg_end = (const char *)memchr(key, sim::MSG_END_BYTE, data_size);
	if(!msg_end){
		return 0;
	}
	int msg_len = msg_end - data + 1;
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
			// 如果最后一个空格被省略, 就会出现 key > msg_end, 否则 key = msg_end
			if(key == msg_end){
				key += 1;
			}
			buffer_offset += msg_len;
			// 一个完整的报文解析结束, 从缓冲区清除已经解析了的数据
			//log_debug("msg.len: %d, buffer.len: %d", msg_len, buffer_.size());
			return 1;
		}
	}
	return 0;
}

}; // namespace sim
