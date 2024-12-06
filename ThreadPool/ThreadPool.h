#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <cassert>
#if defined (_WIN32) || defined (_WIN64)
	#include <Windows.h>
#else
	#include <pthread.h>
#endif

using namespace std;

using func = std::function<void(int)>;

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();

	void addTask(func task);
	bool isEmpty();
	unsigned int getThreadCount();

private:

	struct ThreadParams {
		queue<func> taskQueue;
		mutex queueMutex;
		condition_variable queueCondition;
		bool shutdown = false;
	};

#if defined (_WIN32) || defined (_WIN64)
	vector<HANDLE> threads;
#else
	vector<pthread_t> threads;
#endif

	ThreadParams threadParams;

#if defined (_WIN32) || defined (_WIN64)
	static unsigned __stdcall run(void* param);
#else
	static void* run(void* param);
#endif

	static int getCurrentThreadId();
};