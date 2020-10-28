#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>
#include <bits/shared_ptr_atomic.h>
#include "p_rwlock.h"

using namespace std;

class bar
{
public:
    bar() : num(-1)
    {
        v.clear();
    }
    explicit bar(vector<int> &in) : num(-1)
    {
        for (auto iter : in)
            v.push_back(iter);
    }
    int get_num()
    {
        for (auto iter : v)
        {
            num = max(iter, num);
        }
        usleep(10);
        if (num >= 100)
        {
            printf("Fault %d\n", num);
        }
        return num;
    }
    void update(vector<int> &in)
    {
        for (auto iter : in)
            v.push_back(iter);
    }

private:
    int num;
    vector<int> v;
};

static rw_lock_nr _lock;

shared_ptr<bar> ptr_store;
int max_read_times = 1e4, max_write_times = 200;
int st;
bar bar_1;

void get_func()
{
    for (int k = 0; k <= max_read_times; k++)
    {
        auto temp = atomic_load_explicit(&ptr_store, std::memory_order_seq_cst);
        //printf("Get_num %d\n", temp->get_num());
        temp->get_num();
    }
}

void set_func()
{
    for (int k = 0; k <= max_write_times; k++)
    {
        usleep((max_read_times / max_write_times) / 10);
        vector<int> v;
        int sz = rand() % 100;
        for (int i = 0; i < sz; i++)
            v.push_back(rand() % 100);
        atomic_exchange(&ptr_store, make_shared<bar>(v));
    }
}

void get_func_lock()
{
    auto_read_lock l(_lock);
    for (int k = 0; k <= max_read_times; k++)
    {
        bar_1.get_num();
    }
}

void set_func_lock()
{
    auto_write_lock l(_lock);
    for (int k = 0; k <= max_write_times; k++)
    {
        usleep((max_read_times / max_write_times) / 10);
        vector<int> v;
        int sz = rand() % 100;
        for (int i = 0; i < sz; i++)
            v.push_back(rand() % 100);
        bar_1.update(v);
    }
}

int main()
{
    srand((unsigned)time(NULL));
    vector<int> v = {1, 2, 3};
    ptr_store = make_shared<bar>(v);
    st = clock();
    std::thread t1(get_func);
    std::thread t2(set_func);
    t1.join();
    t2.join();
    printf("spin_lock used time: %ld\n", clock() - st);
    st = clock();
    std::thread t3(get_func_lock);
    std::thread t4(set_func_lock);
    t3.join();
    t4.join();
    printf("rw_lock used time: %ld\n", clock() - st);
}

// g++ atomic_exchange.cpp -o atomic_exchange -pthread && ./atomic_exchange > atomic_exchange.out
