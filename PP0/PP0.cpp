#include <iostream>
#include <vector>
#include <future>
#include <windows.h>
#include <chrono>

using namespace std;

const size_t elemCount = 100000;
const size_t counter = 20;

void quickSort(int* arr, int low, int high);
void quickSortAsync(int* arr, int low, int high);

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    int* arr = new int[elemCount];
    int averageTime = 0;
    int averageAsyncTime = 0;
    for (int i = 1; i <= counter; ++i) {
        cout << "запуск N: " << i << endl;

        for (int i = 0; i < elemCount; ++i)
            arr[i] = rand();

        auto start = chrono::steady_clock::now();
        quickSort(arr, 0, elemCount - 1);
        auto end = chrono::steady_clock::now();

        averageTime += chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout << "время: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;

        for (int i = 0; i < elemCount; ++i)
            arr[i] = rand();

        start = chrono::steady_clock::now();
        quickSortAsync(arr, 0, elemCount - 1);
        end = chrono::steady_clock::now();

        averageAsyncTime += chrono::duration_cast<chrono::milliseconds>(end - start).count();

        cout << "время async: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << endl;
        cout << endl;
    }
    averageAsyncTime /= counter;
    averageTime /= counter;
    delete[] arr;
}

int partition(int* arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);

    return (i + 1);
}

void quickSort(int* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void quickSortAsync(int* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        auto future1 = async(launch::async, quickSort, arr, low, pi - 1);
        auto future2 = async(launch::async, quickSort, arr, pi + 1, high);

        future1.wait();
        future2.wait();
    }
}