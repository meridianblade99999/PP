#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>

using namespace std;

using func = std::function<void(int)>; //std::function<void()>;

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();

	void addTask(func task);
	bool isEmpty();
	int getThreadCount();

private:
	bool shutdown;
	vector<thread> threads;

	queue<func> taskQueue;
	mutex queueMutex;
	condition_variable queueCondition;

	void run();
	int getCurrentThreadId();
};