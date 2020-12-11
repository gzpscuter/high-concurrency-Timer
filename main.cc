/**
 *
 *  main.cc
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#include <iostream>
#include <thread>
#include <atomic>

#include "Timer.h"
#include "delay_queue/TimeEntry.h"
using namespace std;
using namespace timer;

static time_entry<> max_diff;
static atomic<long> impleTaskNum = 0;

void printSth(time_entry<> timeDue)
{
    time_entry<> hello;
    hello.getTimeOfDay();
    max_diff = max_diff < (hello - timeDue) ? (hello - timeDue) : max_diff;
    impleTaskNum++;
    cout << "打印信息　" << hello <<" "<< timeDue <<" diff " << (hello - timeDue)<<" max_diff "<<max_diff<<endl;
}

void t1(Timer* timer)
{
    time_entry<> tt({10, 0});
    time_entry<> due;
    due.getTimeOfDay();
    due = (due + tt);

    for(int i=0; i< 33334; i++) {
        timer->addByDelay(tt, printSth, due);
    }

}

void t2(Timer* timer)
{
    TimerExpiration ttEx(2020, 12, 10, 22, 40,5);
    time_entry<> due = ttEx.getTimeEntry();
    for(int i=0; i< 33334; i++) {
        timer->addByExpiration(ttEx, printSth, due);
    }

}

int main()
{

    time_entry<> tick = {{0, 200000}};
    Timer timer(tick, 20);
    timer.start();

    thread l1(t2, &timer);

    thread l2(t2, &timer);

    thread l3(t2, &timer);

    l1.join();
    l3.join();
    l2.join();


    cin.get();
    cout <<" totally "<<impleTaskNum<<" tasks have been implemented"<<endl;
    impleTaskNum = 0;

    cout<<"now set the second task"<<endl;
    thread l11(t1, &timer);
    thread l22(t1, &timer);
    thread l33(t1, &timer);
    l11.join();
    l22.join();
    l33.join();
    int kk;
    cin>>kk;
    cout <<" totally "<<impleTaskNum<<" tasks have been implemented"<<endl;

    timer.shutdown();
}