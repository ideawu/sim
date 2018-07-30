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

// Thread safe queue
template <class T>
class Queue{
	private:
		pthread_cond_t cond;
		pthread_mutex_t mutex;
		std::queue<T> items;
	public:
		Queue();
		~Queue();

		bool empty();
		int size();
		int push(const T item);
		// TODO: with timeout
		int pop(T *data);
};

template <class T>
Queue<T>::Queue(){
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);
}

template <class T>
Queue<T>::~Queue(){
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}

template <class T>
bool Queue<T>::empty(){
	bool ret = false;
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	ret = items.empty();
	pthread_mutex_unlock(&mutex);
	return ret;
}

template <class T>
int Queue<T>::size(){
	int ret = -1;
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	ret = items.size();
	pthread_mutex_unlock(&mutex);
	return ret;
}

template <class T>
int Queue<T>::push(const T item){
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	{
		items.push(item);
	}
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
	return 1;
}

template <class T>
int Queue<T>::pop(T *data){
	if(pthread_mutex_lock(&mutex) != 0){
		return -1;
	}
	{
		// 必须放在循环中, 因为 pthread_cond_wait 可能抢不到锁而被其它处理了
		while(items.empty()){
			//fprintf(stderr, "%d wait\n", pthread_self());
			if(pthread_cond_wait(&cond, &mutex) != 0){
				//fprintf(stderr, "%s %d -1!\n", __FILE__, __LINE__);
				return -1;
			}
			//fprintf(stderr, "%d wait 2\n", pthread_self());
		}
		*data = items.front();
		//fprintf(stderr, "%d job: %d\n", pthread_self(), (int)*data);
		items.pop();
	}
	if(pthread_mutex_unlock(&mutex) != 0){
		//fprintf(stderr, "error!\n");
		return -1;
	}
		//fprintf(stderr, "%d wait end 2, job: %d\n", pthread_self(), (int)*data);
	return 1;
}

// Selectable queue, multi writers, single reader
template <class T>
class Channel{
private:
	int fds[2];
public:
	Mutex mutex;
	std::queue<T> items;

	Channel();
	~Channel();
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
Channel<T>::Channel(){
	if(pipe(fds) == -1){
		exit(0);
	}
}

template <class T>
Channel<T>::~Channel(){
	close(fds[0]);
	close(fds[1]);
}

template <class T>
int Channel<T>::size(){
	Locking l(&mutex);
	return items.size();
}

template <class T>
int Channel<T>::push(const T item){
	Locking l(&mutex);
	items.push(item);
	if(::write(fds[1], "1", 1) == -1){
		exit(0);
	}
	return 1;
}

template <class T>
int Channel<T>::pop(T *data){
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

