class Buffer
{
public:
	Buffer();
	~Buffer();
	
	int size() const;
	char* data();
	
	int remove(int count);
	int append(char *buf, int len);
	
private:
	int _size;
	char *_buf;
	int _capacity;
};
