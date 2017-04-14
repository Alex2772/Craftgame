#pragma once

#include <functional>
#include <thread>
#include <condition_variable>
#include <queue>

typedef std::function<void()> fn_type;

class Worker {
public:
	Worker();
	~Worker();
	void thread_fn();
	void appendFn(fn_type fn);
	size_t getTaskCount();
	bool isEmpty();
private:
	bool enabled;
	std::thread* thread;
	std::condition_variable cv;
	std::mutex mutex;
	std::queue<fn_type> queue;
};
typedef std::shared_ptr<Worker> worker_ptr;

class ThreadPool {
public:
	ThreadPool(size_t threads = 0);
	template<class _fn, class... _args>
	void runAsync(_fn fn, _args... args) {
		getFreeWorker()->appendFn(std::bind(fn, args...));
	}
	~ThreadPool();
private:
	worker_ptr getFreeWorker();
	std::vector<worker_ptr> workers;
};