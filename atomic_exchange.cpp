#include <bits/stdc++.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <unistd.h>
// #include <bits/shared_ptr_atomic.h>

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
            num = max(iter, num);
        return num;
    }
    ~bar()
    {
        // printf("bar:%d is destroyed\n", num);
    }

private:
    int num;
    vector<int> v;
};

shared_ptr<bar> ptr_store;

int st;

void get_func()
{
    while (1)
    {
        ptr_store->get_num();
    }
}

void set_func()
{
    while (1)
    {
        usleep(10);
        //atomic_exchange(&ptr_store, make_shared<bar>(i));
        vector<int> v;
        default_random_engine e;
        int sz = e() % 100;
        for (int i = 0; i < sz; i++)
            v.push_back(e() % 100);
        ptr_store = make_shared<bar>(v);
    }
}

int main()
{
    vector<int> v = {1, 2, 3};
    ptr_store = make_shared<bar>(v);
    std::thread t1(get_func);
    std::thread t2(set_func);
    t1.join();
    t2.join();
}
