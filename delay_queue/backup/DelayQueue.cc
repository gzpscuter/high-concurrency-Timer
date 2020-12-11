#include "DelayQueue.h"
#include <mutex>

#include <iostream>

using namespace delay_queue;

using namespace std;

template<typename T>
DelayQueue<T>::DelayQueue(void)
{

}

template<typename T>
T DelayQueue<T>::poll()
{
    std::unique_lock<SpinLock> lock(m_spinLock);
    if(!q.empty()){

        delay_queue_entry first = q.top();
        cout <<"dqe first " << first.dqe_sysTime.tv_sec<<" "<<first.dqe_sysTime.tv_usec<<" "<<
        first.dqe_delayTime.tv_sec<<" "<<first.dqe_delayTime.tv_usec<<endl;
        if(getDelay(first)) {
            return {-1L, -1L};
        } 
        else {
            q.pop();
            return first.dqe_delayTime;
        }
    }
    return {-1L, -1L};

}

template<typename T>
bool DelayQueue<T>::offer(T e)
{
    std::unique_lock<SpinLock> lock(m_spinLock);
    cout << "offer success! e.tv_sec "<<e.tv_sec<<" e.tv_usec "<<e.tv_usec<<endl;
    delay_queue_entry dqe_n(e);
    q.emplace(std::move(dqe_n));
    // if(q.top() == e) {

    // }
    return true;
}

template<typename T>
bool DelayQueue<T>::getDelay(DelayQueue<T>::delay_queue_entry& e)
{
    T e_sysTime = e.dqe_sysTime;
    T e_delayTime = e.dqe_delayTime;
    T delay_time = e_sysTime + e_delayTime;
    T now_sysTime;
    gettimeofday(&now_sysTime, NULL);
    e = delay_time - now_sysTime;
    if(e.tv_sec==-1L && e.tv_usec==-1L) return false;
    return true;
}


SpinLock::SpinLock(micro_seconds _SleepWhenAcquireFailedInMicroSeconds = micro_seconds(-1))
:m_SleepWhenAcquireFailedInMicroSeconds(_SleepWhenAcquireFailedInMicroSeconds)
{}

void SpinLock::unlock()
{
    m_locked_flag.store(false);
}

void SpinLock::lock()
{
    bool exp = false;
    while (!m_locked_flag.compare_exchange_strong(exp, true)) {
        exp = false;
        if (m_SleepWhenAcquireFailedInMicroSeconds == micro_seconds(-1)) {
            std::this_thread::yield();
        } else if (m_SleepWhenAcquireFailedInMicroSeconds != 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(m_SleepWhenAcquireFailedInMicroSeconds));
        }
    }
}

