#ifndef SIM_HANDLER_H_
#define SIM_HANDLER_H_

#include <queue>
#include "link.h"
#include "message.h"
#include "../util/thread.h"

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
	}
	~Response(){
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
	
	virtual Response* handle();

protected:
	void push_response(Response *resp);

private:
	SelectableQueue<Response *> resps;
};

}; // namespace sim

#endif
