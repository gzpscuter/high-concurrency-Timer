
#include <iostream>
#include <sys/time.h>
#include <thread>
#include <string>

#include "DelayQueue.h"

using namespace std;
using namespace delay_queue;

DelayQueue<string> kk;
string* hello_echo = new string("nihaoma ");

void t1()
{
    static int hello = 1;
    for(int i=0; i<20; i++) {
        kk.offer(hello_echo,time_entry<>({0, (hello++)*10000000}));
    }
}

int main(int argc, char** argv)
{
    time_entry<> ll({0,0});

    thread l1(t1);
    thread l2(t1);
    l1.join();
    l2.join();

    // for(int i=0; i<5; i++){
    //     thread tmp_the(t1);
    //     tmp_the.join();
    // }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    while(true){
        cout<<"step into while"<<endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        string* sth = kk.poll();
        // if(fuck.tv_sec!=-1L && fuck.tv_usec!=-1L) {
        //     cout << "timer rings! due_time "<<fuck.tv_sec << '\t' << fuck.tv_usec<<endl;
        // }
        cout << *sth << endl;
    }

    // while(true) {
    //     timeval tt = {0, 4000000};
    //     timeval jj = kk.poll();
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     kk.offer(tt);
    // }
}