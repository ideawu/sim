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
	virtual HandlerState proc(const Request &req, Response *resp);
	
	Response* handle();

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
