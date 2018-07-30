#include "transport.h"
#include "util/log.h"
#include "fde.h"
#include "tcp_socket.h"

using namespace sim;

// #define TICK_INTERVAL          100 // ms
// volatile bool quit = false;
// volatile uint32_t g_ticks = 0;
//
// void signal_handler(int sig){
// 	switch(sig){
// 		case SIGTERM:
// 		case SIGINT:{
// 			quit = true;
// 			break;
// 		}
// 		case SIGALRM:{
// 			g_ticks ++;
// 			break;
// 		}
// 	}
// }


Transport::Transport(){
}

Transport::~Transport(){
}

// Link* Transport::listen(){
// 	// while(1){
// 	// 	// _signal.wait();
// 	// 	// Locking l(&_mutex);
// 	//
// 	// 	if(!_waiting_links.empty()){
// 	// 		Link *link = _waiting_links.front();
// 	// 		_waiting_links.pop_front();
// 	// 		return link;
// 	// 	}
// 	// }
// }
//
// void Transport::accept(Link *link){
// 	// Locking l(&_mutex);
//
// 	// _working_links.insert(link);
// 	// link->accept();
// }
//
// void Transport::close(Link *link){
// 	// Locking l(&_mutex);
//
// 	// _working_links.erase(link);
// 	// link->close();
// }

void Transport::setup(){
	pthread_t tid;
	int err = pthread_create(&tid, NULL, &Transport::run, this);
	if(err != 0){
		log_error("can't create thread: %s", strerror(err));
	}
}

void* Transport::run(void *arg){
// 	Transport *trans = (Transport *)arg;
// 	signal(SIGPIPE, SIG_IGN);
// 	signal(SIGINT, signal_handler);
// 	signal(SIGTERM, signal_handler);
// #ifndef __CYGWIN__
// 	signal(SIGALRM, signal_handler);
// 	{
// 		struct itimerval tv;
// 		tv.it_interval.tv_sec = (TICK_INTERVAL / 1000);
// 		tv.it_interval.tv_usec = (TICK_INTERVAL % 1000) * 1000;
// 		tv.it_value.tv_sec = 1;
// 		tv.it_value.tv_usec = 0;
// 		setitimer(ITIMER_REAL, &tv, NULL);
// 	}
// #endif
	
	Fdevents *fdes;
	const Fdevents::events_t *events;
	fdes = new Fdevents();
	
	TcpSocket *tcp_serv = TcpSocket::listen("127.0.0.1", 9000);
	fdes->set(tcp_serv->fd(), FDEVENT_IN, 0, tcp_serv);
	
	log_debug("");
	while(1){
		log_debug("");
		events = fdes->wait(500);
		
		for(int i=0; i<(int)events->size(); i++){
			const Fdevent *fde = events->at(i);
			if(fde->data.ptr == tcp_serv){
				TcpSocket *conn = tcp_serv->accept();
				if(conn){
					log_debug("new connection from %s:%d, fd: %d",
						conn->remote_ip, conn->remote_port, conn->fd());
					fdes->set(conn->fd(), FDEVENT_IN, 1, conn);
				}else{
					log_debug("accept return NULL");
				}
			}else{
				TcpSocket *conn = (TcpSocket *)fde->data.ptr;
				log_debug("read from connection %s:%d, fd: %d",
					conn->remote_ip, conn->remote_port, conn->fd());
			}
		}
		
	}
	return NULL;
}
