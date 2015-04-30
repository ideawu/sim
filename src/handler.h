#ifndef SIM_HANDLER_H_
#define SIM_HANDLER_H_

#include <queue>
#include <map>
#include "link.h"
#include "message.h"
#include "thread.h"

namespace sim{

class Server;

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

typedef enum{
	HANDLE_OK   = 0,
	HANDLE_FAIL = 1,
	HANDLE_RESP = 1,
}HandlerState;

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
	
class Handler
{
public:
	Handler(){};
	virtual ~Handler(){};

	virtual HandlerState accept(const Session &sess);
	virtual HandlerState close(const Session &sess);
	
	// 如果有响应需要立即返回给客户端, 在函数内返回 HANDLE_RESP;
	virtual HandlerState proc(const Request &req, Response *resp);
	//virtual int init();
	//virtual void thread();
	
	virtual int fd(){
		return resps.fd();
	}
	// 当 fd() 有可读事件时, 调用本函数
	virtual Response* handle();
	
	friend class Server;

protected:
	Server *server;
	// 在异步线程中, 返回响应
	void async_send(Response *resp);
	
	HandlerState ok(){ return HANDLE_OK; };
	HandlerState fail(){ return HANDLE_FAIL; };
	HandlerState resp(){ return HANDLE_RESP; };
	
private:
	SelectableQueue<Response *> resps;
};

}; // namespace sim

#endif
