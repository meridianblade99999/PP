#include <iostream>
#include <future>
#include <windows.h>


const size_t gCount = 220;
const size_t gThreads = 4;

void init_array(int* arr)
{
    for (size_t i = 0; i < gCount; i++)
    {
        arr[i] = rand() % 100;
    }
}

int sum(int* arr, size_t begin, size_t end)
{
    printf("callback begin = %d, end = %d\n", begin, end);
    int result = 0;
    for (size_t i = begin; i < end; i++)
    {
        result += arr[i];
    }
    return result;
}

int sum_async_func(int* arr)
{
    // массив будущих результатов (от каждого потока)
    std::future<int> f[gThreads];

    // размер блока массива
    size_t n = gCount / gThreads;
    // создается NTHREAD дополнительных потоков
    for (size_t i = 0; i < gThreads; ++i)
    {
        size_t begin = n * i;
        size_t end = n * (i + 1);

        f[i] = std::async(std::launch::async, sum, arr, begin, end);
    }
    int global_sum_f = 0;

    for (size_t i = 0; i < gThreads; i++)
        global_sum_f += f[i].get();

    //printf("sum_async_func, global_sum_f = %u\n", global_sum_f);
    return global_sum_f;
}

int main()
{
    int arr[gCount];
    // инициализация массива
    srand(GetTickCount());
    init_array(arr);
    // вычисление суммы в одном потоке
    int sum_single_thread = sum(arr, 0, gCount);
    std::cout << "sum_single_thread = " << sum_single_thread << std::endl;
    // вычисление суммы в ассинхронных задачах
    int sum_async_ = sum_async_func(arr);
    std::cout << "sum_async = " << sum_async_ << std::endl;
    return 0;

}