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
	void wait();
	void notify_all(); 
	template<class _Rep,
		class _Period>
		std::cv_status wait_for(const std::chrono::duration<_Rep, _Period>& _Rel_time)
	{
		std::unique_lock<std::mutex> lck(mutex);
		return cv.wait_for(lck, _Rel_time);
	}

	template<class _Rep,
		class _Period,
		class _Predicate>
		bool wait_for(
			const std::chrono::duration<_Rep, _Period>& _Rel_time,
			_Predicate _Pred)
	{
		std::unique_lock<std::mutex> lck(mutex);
		return cv.wait_for(lck, _Rel_time, _Pred);
	}

	void clear();
	std::thread* thread;
private:
	bool enabled;
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
	bool named = false;
	worker_ptr getFreeWorker();
	std::vector<worker_ptr> workers;
};

class AsyncThread {
public:
	AsyncThread();
	template<class _fn, class... _args>
	void runAsync(_fn fn, _args... args) {
		worker->appendFn(std::bind(fn, args...));
	}
	void wait();
	void wait_for();
	void notify();
	void join();

	std::thread::id getId() const;
	void clear()
	{
		worker->clear();
	}
private:
	worker_ptr worker;
};