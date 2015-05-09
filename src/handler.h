#ifndef SIM_HANDLER_H_
#define SIM_HANDLER_H_

#include "link.h"
#include "message.h"
#include "util/thread.h"

namespace sim{

class Session
{
public:
	int64_t id;
	Link *link;
	
	Session(){
		static int64_t inc = 0;
		this->id = inc ++;
		this->link = NULL;
	}
	~Session(){
	}
};

class Request{
public:
	Message msg;
	Session sess;

	double stime;
	double time_wait;
	double time_proc;
};

class Response{
public:
	Message msg;
	Session sess;
};


typedef enum{
	HANDLE_OK   = 0,
	HANDLE_FAIL = 1,
	HANDLE_RESP = 1,
}HandlerState;


class Handler
{
public:
	Handler(){};
	virtual ~Handler(){};

	// 当有新客户端进来时, 此方法被调用. 如果返回 HANDLE_FAIL, 连接将被关闭.
	virtual HandlerState accept(const Session &sess);
	// 当客户端被关闭时, 此方法被调用.
	virtual HandlerState close(const Session &sess);
	
	// 当收到客户端的一个请求报文时, 调用此方法.
	// 如果有响应需要立即返回给客户端, 将响应加到 resp 中, 并返回 HANDLE_RESP;
	virtual HandlerState proc(const Request &req, Response *resp);
	//virtual int init();
	//virtual void thread();
	
	/***** 以下是特殊方法, 你一般不需要关心. *****/
	
	// 此方法默认返回异步响应队列的 fd, 你可以重写此方法, 返回你自己的 fd.
	virtual int fd(){
		return resps.fd();
	}
	// 当 fd() 有可读事件时, 本函数被调用.
	// 如果此方法有响应需要立即返回, 请返回 Response 实例, 外面会负责释放内存.
	// 如无响应, 返回 NULL.
	virtual Response* handle();
	
protected:
	// 将异步响应加入到队列中, 该响应会被发送给客户端.
	// 如果 Handler 是多线程的, 可能会调用本方法将响应发给客户端.
	void async_send(Response *resp);
	
	HandlerState ok(){ return HANDLE_OK; };
	HandlerState fail(){ return HANDLE_FAIL; };
	HandlerState resp(){ return HANDLE_RESP; };

private:
	SelectableQueue<Response *> resps;
};

}; // namespace sim

#endif
