
class Link
{
public:
	int fd() const;
	Buffer* buffer() const;

	virtual int net_read() = 0;
	virtual int net_write() = 0;
	
	virtual int send(Message &msg) = 0;
};


class Server
{
public:
	Link* link() const;
	void proc();
};

class Session
{
public:
	Link* link() const;
	Parser* parser() const;
	
};

/*
Transport:
	# 监控新连接，当有新连接时返回连接 id
	listen() => id, status
	# 接受指定的连接
	accept(id)
	# 关闭连接
	close(id)
	send(id, msg)
	recv() => id, msg

Thread1:
	id, status = listen();
	if(status == new){
		accept(id);
	}
	if(status == closed){
		close(id);
	}

Thread2:
	id, msg = recv();
	resp = process(msg);
	if(resp){
		send(id, resp);
	}

Thread3:
	id, msg = do_business();
	send(id, msg);
*/


