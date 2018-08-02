#ifndef SIM_WORKER_H
#define SIM_WORKER_H

#include "util/thread.h"
#include "util/log.h"

template <class T>
class Worker
{
public:
	Worker();
	virtual ~Worker();
	
	void start(int num=1);
	void stop();
	void add_task(T task);
	
	virtual void process(T task) = 0;
	
private:
	Queue<T> _tasks;
	Mutex _mutex;
	bool _quit;
	int _num;
	static void* run(void *arg);
};

template <class T>
Worker<T>::Worker(){
}

template <class T>
Worker<T>::~Worker(){
}

template <class T>
void Worker<T>::add_task(T task){
	_tasks.push(task);
}

template <class T>
void Worker<T>::start(int num){
	_quit = false;
	_num = num;
	for(int i=0; i<num; i++){
		pthread_t tid;
		int err = pthread_create(&tid, NULL, &Worker::run, this);
		if(err != 0){
			log_error("can't create thread: %s", strerror(err));
		}
	}
}

template <class T>
void Worker<T>::stop(){
	_quit = true;
	
	for(int i=0; i<20; i++){
		usleep(10 * 1000);
		Locking l(&_mutex);
		if(_num == 0){
			break;
		}
	}
}

template <class T>
void* Worker<T>::run(void *arg){
	Worker *worker = (Worker *)arg;
	
	while(!worker->_quit){
		T event;
		int ret = worker->_tasks.pop(&event, 20);
		if(ret == 1){
			worker->process(event);
		}
	}
	
	Locking l(&worker->_mutex);
	worker->_num --;
	
	return NULL;
}
#endif
