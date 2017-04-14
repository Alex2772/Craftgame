#include "ThreadPool.h"

/*
Â THREADPOOL ÍÅËÜÇß ÄÎÁÀÂËßÒÜ ÏÎÒÎÊÈ Ñ ÁÅÑÊÎÍÅ×ÍÛÌÈ ÖÈËÊÀÌÈ
*/

Worker::Worker() :
	enabled(true),
	queue()
{
	thread = new std::thread(&Worker::thread_fn, this);
}
Worker::~Worker() {
	enabled = false;
	cv.notify_one();
	thread->join();
	delete thread;
}
void Worker::appendFn(fn_type fn) {
	std::unique_lock<std::mutex> lck(mutex);
	queue.push(fn);
	cv.notify_one();
}
size_t Worker::getTaskCount() {
	std::unique_lock<std::mutex> lck(mutex);
	return queue.size();
}
bool Worker::isEmpty() {
	std::unique_lock<std::mutex> lck(mutex);
	return queue.empty();
}
void Worker::thread_fn() {
	while (enabled) {
		std::unique_lock<std::mutex> lck(mutex);
		
		cv.wait(lck, [&]() {
			return !queue.empty() || !enabled;
		});
		while (!queue.empty()) {
			fn_type f = queue.front();
			lck.unlock();
			f();
			lck.lock();
			queue.pop();
		}
	}
}

ThreadPool::ThreadPool(size_t threads) {
	if (!threads) {
		threads = 1;
	}
	for (size_t i = 0; i < threads; i++) {
		worker_ptr p(new Worker);
		workers.push_back(p);
	}
}
ThreadPool::~ThreadPool() {
	workers.clear();
}
worker_ptr ThreadPool::getFreeWorker() {
	worker_ptr w;
	size_t min = UINT32_MAX;
	for (auto &it : workers) {
		if (it->isEmpty())
			return it;
		else if (min > it->getTaskCount()) {
			min = it->getTaskCount();
			w = it;
		}
	}
	return w;
}