#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <Windows.h>

using namespace std;

using func = std::function<void(int)>; //std::function<void()>;	

class WindowsThreadPool {
public:
	WindowsThreadPool();
	~WindowsThreadPool();

	void addTask(func task);
	bool isEmpty();
	unsigned int getThreadCount();

private:
	vector<HANDLE> threads;

	static int getCurrentThreadId();
	static unsigned __stdcall run(void* param);

	struct ThreadParams {
		queue<func> taskQueue;
		mutex queueMutex;
		condition_variable queueCondition;
		bool shutdown = false;
	};

	ThreadParams threadParams;
};