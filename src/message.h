#ifndef SIM_MESSAGE_H_
#define SIM_MESSAGE_H_

#include <stdint.h>
#include <inttypes.h>
#include <string>
#include <map>

namespace sim{

class Message{
public:
	Message();
	~Message();
	
	void reset();
	
	std::string type() const;
	void set_type(const std::string &type);
	
	void add(const std::string &val);
	void set(int tag, int32_t val);
	void set(int tag, int64_t val);
	void set(int tag, const char *val);
	void set(int tag, const std::string &val);
	const std::string* get(int tag) const;
	
	std::string encode() const;

private:
	std::string type_;
	std::map<int, std::string> fields_;
	friend class Decoder;
};

}; // namespace sim

#endif
