#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <fstream>
#include "ThreadPool.h"

using namespace std;

const int N = 1000000000;
const string fileName = "out.txt";
mutex fileMutex;

void saveToFile(int result) {
	lock_guard<mutex> lock(fileMutex);
	ofstream file(fileName, ios::app);
	file << result << endl;
	file.close();
}

void f(int a) {
	for (int i = 0; i < N; i++) {
		++a;
		--a;
		int x = a % 100;
		a -= x;
		a += x;
	}
	saveToFile(a);
}

int main()
{	
	ofstream file(fileName);
	file.clear();
	file.close();

	ThreadPool* threadPool = new ThreadPool();;

	for (unsigned int i = 0; i < threadPool->getThreadCount() * 2; i++) {
		func a = std::bind(&f, std::placeholders::_1);
		threadPool->addTask(a);
	}

	cout << "Calculating Trash..." << endl;

	while (!threadPool->isEmpty());

	delete threadPool;

	cout << "Results saved to file: " << fileName << endl;
}