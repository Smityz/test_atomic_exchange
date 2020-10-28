#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>
#include <bits/shared_ptr_atomic.h>
#include "rwlock.h"

using namespace std;

class rw_lock_nr
{
public:
    rw_lock_nr() {}
    ~rw_lock_nr() {}

    __inline void lock_read() { _lock.lockReader(); }
    __inline void unlock_read() { _lock.unlockReader(); }
    __inline bool try_lock_read() { return _lock.tryLockReader(); }

    __inline void lock_write() { _lock.lockWriter(); }
    __inline void unlock_write() { _lock.unlockWriter(); }
    __inline bool try_lock_write() { return _lock.tryLockWriter(); }

private:
    NonRecursiveRWLock _lock;
};

template <typename T>
class auto_lock
{
public:
    auto_lock(T &lock) : _lock(&lock) { _lock->lock(); }
    ~auto_lock() { _lock->unlock(); }

private:
    T *_lock;

    auto_lock(const auto_lock &);
    auto_lock &operator=(const auto_lock &);
};

class auto_read_lock
{
public:
    auto_read_lock(rw_lock_nr &lock) : _lock(&lock) { _lock->lock_read(); }
    ~auto_read_lock() { _lock->unlock_read(); }

private:
    rw_lock_nr *_lock;
};

class auto_write_lock
{
public:
    auto_write_lock(rw_lock_nr &lock) : _lock(&lock) { _lock->lock_write(); }
    ~auto_write_lock() { _lock->unlock_write(); }

private:
    rw_lock_nr *_lock;
};

class bar
{
public:
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

private:
    int num;
    vector<int> v;
};

static rw_lock_nr _lock;

shared_ptr<bar> ptr_store;
int max_read_times = 1e6, max_write_times = 1e3;
int st;

void get_func()
{
    for (int k = 0; k <= max_read_times; k++)
    {
        auto temp = atomic_load_explicit(&ptr_store, std::memory_order_seq_cst);
        printf("Get_num %d\n", temp->get_num());
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
        auto temp = atomic_load_explicit(&ptr_store, std::memory_order_seq_cst);
        printf("Get_num %d\n", temp->get_num());
    }
}

void set_func_lock()
{
    auto_write_lock l(_lock);
    for (int k = 0; k <= max_write_times; k++)
    {
        vector<int> v;
        int sz = rand() % 100;
        for (int i = 0; i < sz; i++)
            v.push_back(rand() % 100);
        atomic_exchange(&ptr_store, make_shared<bar>(v));
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
    printf("spin_lock used time: %ld", clock() - st);
    st = clock();
    std::thread t3(get_func_lock);
    std::thread t4(set_func_lock);
    t3.join();
    t4.join();
    printf("rw_lock used time: %ld", clock() - st);
}

// g++ atomic_exchange.cpp -o atomic_exchange -pthread && ./atomic_exchange > atomic_exchange.out
