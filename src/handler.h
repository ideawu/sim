#ifndef SIM_HANDLER_H_
#define SIM_HANDLER_H_

#include <queue>
#include "link.h"
#include "message.h"
#include "thread.h"

namespace sim{

typedef enum{
	HANDLE_OK   = 0,
	HANDLE_FAIL = 1,
	HANDLE_RESP = 1,
}HandlerState;

class Request{
public:
	Message msg;
	Link *link;

	double stime;
	double time_wait;
	double time_proc;
};

class Response{
public:
	Message msg;
	Link *link;
};
	
class Handler
{
public:
	Handler(){};
	virtual ~Handler(){};

	virtual int fd(){
		return resps.fd();
	}
	virtual HandlerState accept(Link *link);
	virtual HandlerState close(Link *link);
	// 如果有响应需要立即返回给客户端, 在函数内返回 HANDLE_RESP;
	virtual HandlerState proc(const Request &req, Response *resp);
	
	// 当 fd() 有可读事件时, 调用本函数
	Response* handle();
	
	/*
	如果想在主线程内读取文件, 或者你想实现一个其它协议的 handler, 你需要:
	1. 重写 fd() 方法, 返回 socket
	2. 重写 handler() 方法, 在 handler() 里进行 read()
	*/

protected:
	void push_response(Response *resp);
	HandlerState ok(){ return HANDLE_OK; };
	HandlerState fail(){ return HANDLE_FAIL; };
	HandlerState resp(){ return HANDLE_RESP; };

private:
	SelectableQueue<Response *> resps;
};

}; // namespace sim

#endif
