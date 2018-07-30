/*
Copyright (c) 2012-2014 The SSDB Authors. All rights reserved.
Use of this source code is governed by a BSD-style license that can be
found in the LICENSE file.
*/
#ifndef SIM_UTIL_THREAD_H_
#define SIM_UTIL_THREAD_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <queue>
#include <vector>

class Mutex{
private:
	pthread_mutex_t mutex;
public:
	Mutex(){
		pthread_mutex_init(&mutex, NULL);
	}
	~Mutex(){
		pthread_mutex_destroy(&mutex);
	}
	void lock(){
		pthread_mutex_lock(&mutex);
	}
	void unlock(){
		pthread_mutex_unlock(&mutex);
	}
};

class Semaphore{
private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
	int _count;
public:
	Semaphore(){
		_count = 0;
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_cond, NULL);
	}
	~Semaphore(){
		pthread_cond_destroy(&_cond);
		pthread_mutex_destroy(&_mutex);
	}
	void wait(){
		pthread_mutex_lock(&_mutex);
		while(_count == 0){
			pthread_cond_wait(&_cond, &_mutex);
		}
		_count--;
		pthread_mutex_unlock(&_mutex);
	}
	void notify(){
		pthread_mutex_lock(&_mutex);
		_count++;
		pthread_mutex_unlock(&_mutex);
		pthread_cond_signal(&_cond);
	}
};

class Locking{
private:
	Mutex *mutex;
	// No copying allowed
	Locking(const Locking&);
	void operator=(const Locking&);
public:
	Locking(Mutex *mutex){
		this->mutex = mutex;
		this->mutex->lock();
	}
	~Locking(){
		this->mutex->unlock();
	}

};

// Selectable queue, multi writers, single reader
template <class T>
class SelectableQueue{
private:
	int fds[2];
public:
	Mutex mutex;
	std::queue<T> items;

	SelectableQueue();
	~SelectableQueue();
	int fd(){
		return fds[0];
	}
	int size();
	// multi writer
	int push(const T item);
	// single reader
	int pop(T *data);
};


template <class T>
SelectableQueue<T>::SelectableQueue(){
	if(pipe(fds) == -1){
		exit(0);
	}
}

template <class T>
SelectableQueue<T>::~SelectableQueue(){
	close(fds[0]);
	close(fds[1]);
}

template <class T>
int SelectableQueue<T>::size(){
	Locking l(&mutex);
	return items.size();
}

template <class T>
int SelectableQueue<T>::push(const T item){
	Locking l(&mutex);
	items.push(item);
	if(::write(fds[1], "1", 1) == -1){
		exit(0);
	}
	return 1;
}

template <class T>
int SelectableQueue<T>::pop(T *data){
	int n, ret = 1;
	char buf[1];

	while(1){
		n = ::read(fds[0], buf, 1);
		if(n < 0){
			if(errno == EINTR){
				continue;
			}else{
				return -1;
			}
		}else if(n == 0){
			ret = -1;
		}else{
			Locking l(&mutex);
			if(items.empty()){
				fprintf(stderr, "%s %d error!\n", __FILE__, __LINE__);
				return -1;
			}
			*data = items.front();
			items.pop();
		}
		break;
	}
	return ret;
}

#endif

