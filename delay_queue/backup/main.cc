
#include <iostream>
#include <sys/time.h>
#include <thread>

#include "DelayQueue.h"

using namespace std;
using namespace delay_queue;

int main(int argc, char** argv)
{
    timeval ll = {0,0};
    DelayQueue<timeval> kk;
    kk.offer({32,444});
    // while(true) {
    //     timeval tt = {0, 4000000};
    //     timeval jj = kk.poll();
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     kk.offer(tt);
    // }
}