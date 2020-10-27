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
    explicit bar(int x) : num(x), a(3), b(2)
    {
    }
    int get_num()
    {
        a += b;
        usleep(10);
        b += a;
        usleep(10);
        b += a + b;
        if (abs(a) > 100)
            a = 0;
        if (abs(b) > 100)
            b = 0;
        return num;
    }
    ~bar()
    {
        // printf("bar:%d is destroyed\n", num);
    }

private:
    int num, a, b;
};

vector<int> v;

shared_ptr<bar> ptr_store;

int st;

void get_func()
{
    for (int t = 0; t <= 10; t++)
    {
        st = clock();
        for (int i = 0; i <= 1000; i++)
        {
            ptr_store->get_num();
        }
        printf("lock version: %ld\n", clock() - st);
    }
}

void set_func()
{
    for (int i = 0; i < 10000; i++)
    {
        usleep(10);
        //atomic_exchange(&ptr_store, make_shared<bar>(i));
        ptr_store = make_shared<bar>(i);
    }
}

int main()
{
    st = clock();
    auto pt = make_shared<bar>(1);
    for (int i = 0; i <= 1000; i++)
    {
        pt->get_num();
    }
    printf("free version: %ld\n", clock() - st);
    ptr_store = make_shared<bar>(-1);
    std::thread t1(get_func);
    std::thread t2(set_func);
    t1.join();
    t2.join();
}
