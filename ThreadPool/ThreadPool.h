#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

using namespace std;

typedef std::function<void()> func;

class ThreadPool {
public:
	ThreadPool();
	~ThreadPool();

	void addTask(func task);

private:
	bool shutdown;
	vector<thread> threads;

	queue<func> queue;
	mutex queueMutex;
	condition_variable queueCondition;

	void run();

};

