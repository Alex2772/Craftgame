#pragma once

#include <vector>
#include <mutex>

template<typename _T>
class AsyncVector :public std::vector<_T> {
public:
	AsyncVector() {}
	virtual void push_back(_T d) {
		std::unique_lock<std::mutex> lck(mutex);
		std::vector<_T>::push_back(d);
	}
	virtual void clear() {
		std::unique_lock<std::mutex> lck(mutex);
		std::vector<_T>::clear();
	}
	virtual _T front() {
		std::unique_lock<std::mutex> lck(mutex);
		return std::vector<_T>::front();
	}
	virtual size_t size() {
		std::unique_lock<std::mutex> lck(mutex);
		return std::vector<_T>::size();
	}
	virtual bool empty() {
		std::unique_lock<std::mutex> lck(mutex);
		return std::vector<_T>::empty();
	}
	std::mutex mutex;
};