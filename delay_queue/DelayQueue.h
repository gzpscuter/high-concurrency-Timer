/**
 *
 *  DelayQueue.h
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#pragma once

#include <queue>
#include <atomic>
#include <thread>
#include <queue>
#include <vector>
#include <sys/time.h>
#include <mutex>

#include "TimeEntry.h"

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
        /// SpinLock的接口说明
        ///
        ///     @brief 该接口是SpinLock类的构造函数
        ///     @param _SleepWhenAcquireFailedInMicroSeconds 未抢到锁时线程阻塞的时间（微秒级），-1时表示不延时而直接让出时间片
        SpinLock(micro_seconds _SleepWhenAcquireFailedInMicroSeconds = micro_seconds(-1));
        /// lock的接口说明
        ///
        /// @brief 该接口通过原子变量m_locked_flag的cas操作实现线程互斥，供std::unique_lock构造函数调用
        void lock();

        void unlock();
    };


    typedef struct delay_queue_entry {
        typedef struct time_entry<>     __type;
        /// dqe_expiration是到期时间
        __type dqe_expiration;
        /// appendix是需要返回的载体
        void* appendix = nullptr;

        delay_queue_entry(void* appen, __type expiration)
        :dqe_expiration(expiration),
        appendix(appen)
        {} ;

        inline bool operator>(const delay_queue_entry& rdqe) const {

            return dqe_expiration.__node.tv_sec*1000000 + dqe_expiration.__node.tv_usec 
            > rdqe.dqe_expiration.__node.tv_sec*1000000 + rdqe.dqe_expiration.__node.tv_usec;            
        }


    } delay_queue_entry;

    /// 延时队列DelayQueue的实现
    ///
    /// @brief 利用自旋锁和stl优先队列实现延时队列，最小延时时间的队列元素排在最前面（小顶堆）
    ///
    template<typename T>
    class DelayQueue {
    public:

        DelayQueue(void) {};

        /// poll的接口说明
        ///
        /// @brief 这个接口根据系统时间判断延时队列是否任务到期
        /// @return 最近任务到期，返回到时任务载体；未到期或者队列无任务，则返回nullptr
        T* poll() {
            std::unique_lock<SpinLock> lock(m_spinLock);
            if(!q.empty()){

                delay_queue_entry first = q.top();
                if(getDelay(first)) {
                    return nullptr;
                } 
                else {
                    q.pop();
                    return static_cast<T*>(first.appendix);
                    // return (first.dqe_dueTime - first.dqe_startTime).getTimeval();
                }
            }
            return nullptr;
        }
        /// offer的接口说明
        ///
        /// @brief 这个接口用于向延时队列添加定时任务
        /// @param timeval e　是定时任务距离该时刻系统时间的时间差，类型为timeval
        /// @return 成功返回true
        bool offer(T* e, time_entry<> expiration = time_entry<>({-1L, -1L})) {
            std::unique_lock<SpinLock> lock(m_spinLock);
            // delay_queue_entry dqe_n(e);
            q.push(delay_queue_entry(e, expiration));
            // if(q.top() == e) {

            // }
            return true;
        }
        
    private:

        SpinLock m_spinLock;     
        std::priority_queue<delay_queue_entry, std::vector<delay_queue_entry>, std::greater<delay_queue_entry> > q;

        bool getDelay(const delay_queue_entry& e) {
            time_entry<> tmp_now;
            tmp_now.getTimeOfDay();
            if(e.dqe_expiration - tmp_now > time_entry<>({0, 0})) return true;
            return false;
        }

        DelayQueue& operator=(const DelayQueue&) = delete;
        DelayQueue(const DelayQueue&) = delete;
    };

}