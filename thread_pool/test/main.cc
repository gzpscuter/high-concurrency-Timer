#include <iostream>
#include "ThreadPool.h"
using namespace std;

void test_func()
{
    static int kk = 0;
    cout << "test " << kk++ << endl;
}

int main(int argc, char** argv)
{
    thread_pool::ThreadPool tp(8);
    for(int i=0; i<20; i++)
        tp.addTask(test_func);
    // cin.get();
}