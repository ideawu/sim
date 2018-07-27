#ifndef SIM_BUFFER_H
#define SIM_BUFFER_H

class Buffer
{
public:
	Buffer();
	~Buffer();
	
	int size() const;
	char* data();
	
	int remove(int count);
	int append(const char *buf, int len);
	
private:
	int _size;
	char *_buf;
	int _capacity;
};

#endif
