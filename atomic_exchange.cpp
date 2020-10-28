#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>
#include <bits/shared_ptr_atomic.h>

using namespace std;

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

shared_ptr<bar> ptr_store;

int st;

void get_func()
{
    for (int k = 0; k <= 10000; k++)
    {
        auto temp = atomic_load_explicit(&ptr_store, std::memory_order_seq_cst);
        printf("Get_num %d\n", temp->get_num());
    }
}

void set_func()
{
    for (int k = 0; k <= 10000; k++)
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
    std::thread t1(get_func);
    std::thread t2(set_func);
    t1.join();
    t2.join();
}

// g++ atomic_exchange.cpp -o atomic_exchange -pthread && ./atomic_exchange > atomic_exchange.out
