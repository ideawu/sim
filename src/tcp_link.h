class Buffer
{
public:
};

class Link
{
public:
	int fd() const;
	Buffer* buffer() const;

	virtual int net_read() = 0;
	virtual int net_write() = 0;
	
	virtual int send(Message &msg) = 0;
};

class Parser
{
public:
	virtual Message* parse(Buffer *buffer) = 0;
};

class Message
{
public:
	virtual Buffer* encode() = 0;
};

class Server
{
public:
	Link* link() const;
	Parser* parser() const;
};

