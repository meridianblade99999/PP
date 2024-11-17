#include "ThreadPool.h"
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <sstream>

using namespace std;

ThreadPool::ThreadPool() {
	shutdown = false;
	int threadCount = std::thread::hardware_concurrency();
	for (int i = 0; i < threadCount; i++) {
		threads.emplace_back(thread(&ThreadPool::run, this));
	}
	cout << "ThreadPool started, threads: " << threadCount << endl;
}

ThreadPool::~ThreadPool() {
	shutdown = true;
	queueCondition.notify_all();
	for (int i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
			threads[i].join();
	}
	cout << "ThreadPool shutdown" << endl;
}

bool ThreadPool::isEmpty() {
	return queue.empty();
}

void ThreadPool::addTask(func task) {
	lock_guard<mutex> lock(queueMutex);
	queue.push(task);
	queueCondition.notify_one();
}

void ThreadPool::run() {
	int threadId = getCurrentThreadId();

	while (!shutdown) {
		unique_lock<mutex> lock(queueMutex);
		queueCondition.wait(lock, [this]() {return !queue.empty() || shutdown;});
		if (!shutdown && !queue.empty()) {
			func f = std::move(queue.front());
			queue.pop();
			lock.unlock();
			f(threadId);
		}
	}
}

int ThreadPool::getCurrentThreadId() {
	std::stringstream ss;

	ss << std::this_thread::get_id();

	int id;
	ss >> id;

	return id;
}

int ThreadPool::getThreadCount() {
	return threads.size();
}
