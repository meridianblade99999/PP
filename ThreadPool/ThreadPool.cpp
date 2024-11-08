#include "ThreadPool.h"
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <queue>

using namespace std;

ThreadPool::ThreadPool() {
	this->shutdown = false;
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
		threads[i].join();
	}
	cout << "ThreadPool shutdown" << endl;
}

void ThreadPool::addTask(func task) {
	lock_guard<mutex> lock(queueMutex);
	queue.push(task);
	queueCondition.notify_one();
}

void ThreadPool::run() {
	while (!shutdown) {
		unique_lock<mutex> lock(queueMutex);
		queueCondition.wait(lock, [&]() {return !queue.empty();});
		if (!shutdown && !queue.empty()) {
			func f = queue.front();
			queue.pop();
			lock.unlock();
			f();
		}
	}
}