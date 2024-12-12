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
	unsigned int threadCount = std::thread::hardware_concurrency();
	for (unsigned int i = 0; i < threadCount; i++) {
		threads.emplace_back(thread(&ThreadPool::run, this));
	}
	cout << "ThreadPool started, threads: " << threadCount << endl;
}

ThreadPool::~ThreadPool() {
	shutdown = true;
	queueCondition.notify_all();
	for (unsigned int i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
			threads[i].join();
	}
	cout << "ThreadPool shutdown" << endl;
}

bool ThreadPool::isEmpty() {
	return taskQueue.empty();
}

void ThreadPool::addTask(func task) {
	lock_guard<mutex> lock(queueMutex);
	taskQueue.push(task);
	queueCondition.notify_one();
}

void ThreadPool::run() {
	int threadId = getCurrentThreadId();

	while (!shutdown) {
		unique_lock<mutex> lock(queueMutex);
		queueCondition.wait(lock, [this]() {return !taskQueue.empty() || shutdown;});
		if (!shutdown && !taskQueue.empty()) {
			func f = std::move(taskQueue.front());
			taskQueue.pop();
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

unsigned int ThreadPool::getThreadCount() {
	return (unsigned int) threads.size();
}
