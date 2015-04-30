#include "../util/strings.h"
#include "sim.h"

namespace sim{

Message::Message(){
}

Message::~Message(){
}

void Message::reset(){
	fields_.clear();
}

std::string Message::type() const{
	return type_;
}

void Message::set_type(const std::string &type){
	type_ = type;
}

void Message::set(int tag, int32_t val){
	this->set(tag, (int64_t)val);
}

void Message::set(int tag, int64_t val){
	this->set(tag, str(val));
}

void Message::set(int tag, const char *val){
	this->set(tag, str(val));
}

void Message::set(int tag, const std::string &val){
	if(tag == 0){
		this->set_type(val);
	}else{
		fields_[tag] = val;
	}
}

void Message::add(const std::string &val){
	int tag;
	std::map<int, std::string>::const_reverse_iterator it;
	it = fields_.rbegin();
	if(it == fields_.rend()){
		tag = 0;
	}else{
		tag = it->first + 1;
	}
	this->set(tag, val);
}

const std::string* Message::get(int tag) const{
	std::map<int, std::string>::const_iterator it;
	it = fields_.find(tag);
	if(it == fields_.end()){
		return NULL;
	}
	return &(it->second);
}

static std::string encode_field(int tag, const std::string &val){
	std::string buffer;
	buffer.append(str(tag));
	buffer.push_back(sim::KV_SEP_BYTE);
	buffer.append(sim::encode(val));
	buffer.push_back(sim::KV_END_BYTE);
	return buffer;
}

std::string Message::encode() const{
	std::string buffer;
	buffer.append(encode_field(0, this->type()));
	std::map<int, std::string>::const_iterator it;
	for(it=fields_.begin(); it!=fields_.end(); it++){
		int tag = it->first;
		if(tag == 0){
			continue;
		}
		const std::string &val = it->second;
		buffer.append(encode_field(tag, val));
	}
	buffer.push_back(sim::MSG_END_BYTE);
	return buffer;
}

}; // namespace sim
