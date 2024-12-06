#include "ThreadPool.h"
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <sstream>

using namespace std;

ThreadPool::ThreadPool() {
	threadParams.shutdown = false;
	unsigned int threadCount = std::thread::hardware_concurrency();
	if (threadCount == 0)
		threadCount = 1;
	for (unsigned int i = 0; i < threadCount; i++) {
		#if defined (_WIN32) || defined (_WIN64)
			unsigned int threadID;
			HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, &run, &threadParams, 0, &threadID);
			threads.emplace_back(thread);
		#else
			pthread_t thread;
			pthread_create(&thread, nullptr, &run, &threadParams);
			printf("pthread is created \n");
			threads.emplace_back(thread);
		#endif
	}
	cout << "ThreadPool started, threads: " << threadCount << endl;
}

ThreadPool::~ThreadPool() {
	threadParams.shutdown = true;
	threadParams.queueCondition.notify_all();
	#if defined (_WIN32) || defined (_WIN64)
		WaitForMultipleObjects(getThreadCount(), threads.data(), TRUE, INFINITE);
	#endif
	for (unsigned int i = 0; i < threads.size(); i++)
	{
		#if defined (_WIN32) || defined (_WIN64)
			for (unsigned int i = 0; i < getThreadCount(); i++)
			{
				CloseHandle(threads[i]);
			}
		#else
			pthread_join(threads[i], nullptr);
		#endif
	}
	cout << "ThreadPool shutdown" << endl;
}

bool ThreadPool::isEmpty() {
	return threadParams.taskQueue.empty();
}

void ThreadPool::addTask(func task) {
	lock_guard<mutex> lock(threadParams.queueMutex);
	threadParams.taskQueue.push(task);
	threadParams.queueCondition.notify_one();
}

#if defined (_WIN32) || defined (_WIN64)
	unsigned __stdcall ThreadPool::run(void* param) {
#else
	void* ThreadPool::run(void* param) {
#endif
		assert(nullptr != param);
		ThreadParams* params = static_cast<ThreadParams*>(param);
		int threadId = getCurrentThreadId();

		while (!params->shutdown) {
			unique_lock<mutex> lock(params->queueMutex);
			params->queueCondition.wait(lock, [params]() {
				return !params->taskQueue.empty() || params->shutdown;
				});
			if (!params->shutdown && !params->taskQueue.empty()) {
				func f = std::move(params->taskQueue.front());
				params->taskQueue.pop();
				lock.unlock();
				f(threadId);
			}
	}

	#if defined (_WIN32) || defined (_WIN64)
		return 0;
	#else
		return nullptr;
	#endif
	}

int ThreadPool::getCurrentThreadId() {
#if defined (_WIN32) || defined (_WIN64)
	std::stringstream ss;

	ss << std::this_thread::get_id();

	int id;
	ss >> id;

	return id;
#else
	return (int) pthread_self();
#endif
}

unsigned int ThreadPool::getThreadCount() {
	return (unsigned int) threads.size();
}
