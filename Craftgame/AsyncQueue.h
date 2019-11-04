#pragma once

#include <queue>
#include <mutex>

template<typename _T>
class AsyncQueue : std::queue<_T> {
public:
	AsyncQueue() {}
	virtual void pop() {
		std::unique_lock<std::mutex> lck(mutex);
		std::queue<_T>::pop();
	}
	virtual void push(_T d) {
		std::unique_lock<std::mutex> lck(mutex);
		std::queue<_T>::push(d);
	}
	virtual _T front() {
		std::unique_lock<std::mutex> lck(mutex);
		return std::queue<_T>::front();
	}
	virtual size_t size() {
		std::unique_lock<std::mutex> lck(mutex);
		return std::queue<_T>::size();
	}
	virtual bool empty() {
		std::unique_lock<std::mutex> lck(mutex);
		return std::queue<_T>::empty();
	}
	std::mutex mutex;
};