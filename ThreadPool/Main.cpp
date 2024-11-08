#include <iostream>
#include "ThreadPool.h"
#include <chrono>
#include <thread>
#include <functional>

using namespace std;

void f(int a) {
	while (1) {
		printf("%d\n", a);
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

int main()
{
	ThreadPool threadPool;

	for (int i = 0; i < 10; i++) {
		std::function<void()> a = std::bind(f, i);
		threadPool.addTask(a);
	}

	this_thread::sleep_for(chrono::milliseconds(100000));
}