#ifndef SIM_HANDLER_H_
#define SIM_HANDLER_H_

#include "link.h"
#include "message.h"

namespace sim{

typedef enum{
	HANDLE_OK   = 0,
	HANDLE_FAIL = 1,
	HANDLE_RESP = 1,
}HandlerState;

class Response{
public:
	Message msg;
	Link *link;
	Response(){
		link = NULL;
	}
	~Response(){
		delete link;
	}
};
	
class Handler
{
public:
	Handler(){};
	virtual ~Handler(){};

	virtual int fd(){
		return -1;
	}
	virtual HandlerState accept(Link *link);
	virtual HandlerState close(Link *link);
	virtual HandlerState proc(Link *link, const Message &msg);
	
	// 当有响应需要发送给某个客户端时, 返回响应的指针. 调用者负责释放.
	virtual Response* handle();

protected:
	void push_response();
	Response* pop_response();
};

}; // namespace sim

#endif
