
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
	virtual ParseStatus parse(Buffer *buffer) = 0;
};

class ParseStatus
{
public:
	Message* message();
	
	bool ok() const;
	bool none() const;
	bool error() const;
	
	void set_ok(Message *msg);
	void set_none();
	void set_error();
	
private:
	int code;
	Message *msg;
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
};

class Session
{
public:
	Link* link() const;
	Parser* parser() const;
};

