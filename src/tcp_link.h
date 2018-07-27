
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

// !!! 多线程时，必须用引用计数，否则无法判断何时释放内存
// std::shared_ptr?

// TODO:
// 服务器循环
while(1){
	// 处理定时器事件
	foreach(timers as timer){
		timer.invoke();
	}
	
	// 处理网络读事件
	foreach(in_links as link){
		lock(link){
			if(link.net_read() == -1){
				break_lock;
			}
			while(req = link.recv()){
				req_queue.add(req);
			}
		}
	}
	
	// 处理响应
	while(link = out_links.pop()){
		lock(link){
			if(link.closed()){
				break_lock;
			}
			link.flush();
		}
	}
}

async proc_recv(){
	while(req = req_queue.pop()){
		resp = serv.proc(req);
		lock(link){
			if(link.closed()){
				break_lock;
			}
			link.send(resp);
			out_links.add_once(link);
		}
	}
}

async business(){
	while(event = pull_event()){
		msg = generate_resp();
		lock(link){
			if(link.closed()){
				break_lock;
			}
			link.send(msg);
			out_links.add_once(link);
		}
	}
}

/*
*/


