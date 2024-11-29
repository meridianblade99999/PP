#include "WindowsThreadPool.h"

#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <sstream>

using namespace std;

WindowsThreadPool::WindowsThreadPool() {
	threadParams.shutdown = false;
	unsigned int threadCount = std::thread::hardware_concurrency();
	for (unsigned int i = 0; i < threadCount; i++) {
		unsigned int threadID;
		HANDLE thread = (HANDLE) _beginthreadex(NULL, 0, &run, &threadParams, 0, &threadID);
		threads.emplace_back(thread);
	}
	cout << "ThreadPool started, threads: " << threadCount << endl;
}

WindowsThreadPool::~WindowsThreadPool() {
	threadParams.shutdown = true;
	threadParams.queueCondition.notify_all();
	WaitForMultipleObjects(getThreadCount(), threads.data(), TRUE, INFINITE);
	for (unsigned int i = 0; i < getThreadCount(); i++)
	{
		CloseHandle(threads[i]);
	}
	cout << "ThreadPool shutdown" << endl;
}

bool WindowsThreadPool::isEmpty() {
	return threadParams.taskQueue.empty();
}

void WindowsThreadPool::addTask(func task) {
	lock_guard<mutex> lock(threadParams.queueMutex);
	threadParams.taskQueue.push(task);
	threadParams.queueCondition.notify_one();
}

unsigned __stdcall WindowsThreadPool::run(void* param) {
	ThreadParams* params = static_cast<ThreadParams*>(param);
	int threadId = getCurrentThreadId();

	while (!params->shutdown) {
		unique_lock<mutex> lock(params->queueMutex);
		params->queueCondition.wait(lock, [params]() {return !params->taskQueue.empty() || params->shutdown; });
		if (!params->shutdown && !params->taskQueue.empty()) {
			func f = std::move(params->taskQueue.front());
			params->taskQueue.pop();
			lock.unlock();
			f(threadId);
		}
	}

	return 0;
}

int WindowsThreadPool::getCurrentThreadId() {
	std::stringstream ss;

	ss << std::this_thread::get_id();

	int id;
	ss >> id;

	return id;
}

unsigned int WindowsThreadPool::getThreadCount() {
	return (unsigned int) threads.size();
}
