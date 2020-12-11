#pragma once

#include <queue>
#include <atomic>
#include <thread>
#include <queue>
#include <vector>
// #include <ctime>
#include <sys/time.h>

namespace delay_queue {
    /// 自旋锁SpinLock的实现
    ///
    ///　   @brief 利用cas操作实现的自旋锁，争抢不到锁的线程会让出时间片（或阻塞一段微秒级时间）
    ///     @param _SleepWhenAcquireFailedInMicroSeconds 未抢到锁时线程阻塞的时间（微秒级），-1时表示不延时而直接让出时间片
    class SpinLock
    {
        typedef size_t micro_seconds;

        std::atomic<bool> m_locked_flag = ATOMIC_VAR_INIT(false);
        micro_seconds m_SleepWhenAcquireFailedInMicroSeconds;

        SpinLock& operator=(const SpinLock) = delete;
        SpinLock(const SpinLock&) = delete;
    public:
        SpinLock(micro_seconds _SleepWhenAcquireFailedInMicroSeconds);
        void lock();
        void unlock();
    };



    /// 延时队列DelayQueue的实现
    ///
    /// @brief 利用自旋锁和stl优先队列实现延时队列，最小延时时间的队列元素排在最前面（小顶堆）
    /// @brief 目前默认模板变量Ｔ为timeval,生而为人，我很抱歉
    template<typename T>
    class DelayQueue {
    public:
        typedef struct delay_queue_entry {
            T dqe_sysTime;
            T dqe_delayTime;

            delay_queue_entry(T delayTime)
            :dqe_delayTime(delayTime)
            {
                gettimeofday(&dqe_sysTime, NULL);
            }


            inline bool operator>(const delay_queue_entry& rdqe) const {
                return dqe_delayTime.tv_sec*1000000 + dqe_delayTime.tv_usec 
                > rdqe.dqe_delayTime.tv_sec*1000000 + rdqe.dqe_delayTime.tv_usec;
            }
        } delay_queue_entry;

        DelayQueue(void);

        T poll();
        bool offer(T e);
        // T put();

        friend T operator-(const T& lval, const T& rval) {
            long sum = lval.tv_sec*1000000 + lval.tv_usec - rval.tv_sec*1000000 - rval.tv_usec;
            if(sum < 0)
                return {-1L, -1L};
            T ans = {sum/1000000, sum%1000000};
            return ans;
        }

        friend T operator+(const T& lval, const T& rval) {
            long sum = lval.tv_sec*1000000 + lval.tv_usec + rval.tv_sec*1000000 + rval.tv_usec;
            T ans = {sum/1000000, sum%1000000};
            return ans;            
        }

        
    private:

        SpinLock m_spinLock;
        // T m_lastSysTime;       
        std::priority_queue<delay_queue_entry, std::vector<delay_queue_entry>, std::greater<delay_queue_entry> > q;

        bool getDelay(delay_queue_entry& e);

        DelayQueue& operator=(const DelayQueue&) = delete;
        DelayQueue(const DelayQueue&) = delete;
    };

}