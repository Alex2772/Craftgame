#include "ThreadPool.h"
#include "GameEngine.h"

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

void Worker::wait()
{
	std::unique_lock<std::mutex> lck(mutex);
	cv.wait(lck);
}

void Worker::notify_all()
{
	cv.notify_all();
}

void Worker::clear()
{
	std::unique_lock<std::mutex> lck(mutex);
	while (!queue.empty())
		queue.pop();
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
	if (!named)
	{
		named = true;
		for (size_t i = 0; i < workers.size(); i++) {
			CGE::instance->logger->setThreadName(workers[i]->thread->get_id(), std::string("thread pool #") + std::to_string(i));
		}
	}
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

AsyncThread::AsyncThread()
{
	worker = std::make_shared<Worker>();
}

void AsyncThread::wait()
{
	worker->wait();
}

void AsyncThread::notify()
{
	worker->notify_all();
}

void AsyncThread::join()
{
	worker->thread->join();
}

std::thread::id AsyncThread::getId() const
{
	return worker->thread->get_id();
}