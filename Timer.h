/**
 *
 *  Timer.h
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#pragma once

#include <iostream>
#include <atomic>
#include <functional>
#include <list>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <memory>

#include "Ticker.h"
#include "TimerTaskList.h"
#include "TimingWheel.h"
#include "thread_pool/ThreadPool.h"
#include "thread_pool/NonCopyable.h"
#include "delay_queue/DelayQueue.h"
#include "delay_queue/TimeEntry.h"

namespace timer {

    class TimerTask;
    class Ticker;
    class TimerTaskEntry;
    class TimerTaskList;
    class TimingWheel;

    /**
     * Timer的构造函数说明
     * 
     * @brief Timer类用于生成定时器，内部包含时钟，延时队列，线程池，时间轮等组件
     * @param tickMs 一个时间轮一个刻度的时间间隔，类型为time_entry<>，精确到微秒
     * @param wheelSize 分级时间轮每一层时间轮所含刻度数目，类型为int
     * @param tickerNum 内部时钟的数目，时钟每次走动都会调用delayqueue.poll()
     * @param timeout 定时器内部时钟Ticker的触发间隔，类型time_entry<>，默认是{0,200000}
     * @param threadNum 线程池的线程消费者线程数目，默认是４
     * 
    */    
    class Timer : public thread_pool::NonCopyable {
    public:

        explicit Timer(time_entry<> tickMs, int wheelSize, int tickerNum = 2, time_entry<> timeout = time_entry<>({0,20000}), int threadNum = 8);
        ~Timer();
        void advanceClock();
        int size();
        /**
         * shutdown的函数说明
         * 
         * @brief 这个函数用于关闭定时器，调用后，定时器内部所有组件都将停止动作，并将线程池剩余任务执行完成后才退出该函数
         * 注意，该函数调用完成后，一定要清理Timer实例，这样才能彻底释放定时器各组件占用的内存
         * 
        */
        void shutdown();
        /**
         * start的函数说明
         * 
         * @brief 这个函数用于启动定时器，在构建完成定时器实例后，即可调用该函数以启动定时器，启动后的定时器才能接受延时任务和
         * 定时任务。
        */
        void start();
        void addTimerTaskEntry(TimerTaskEntry* timerTaskEntry);
        /**
         * addByDelay的函数说明
         * 
         * @brief 这个函数用于向定时器加入延时任务，如果延时时间到，则执行该任务。
         * @param func 参数模板，接收任意函数名
         * @param args 可变模板参数，接收待传入func函数的所有参数
         * @param delayTime 延时时间，类型是time_entry<>
         *
        */
        template<typename Func, typename ... Args>
        void addByDelay(time_entry<> delayTime, const Func& func, Args&& ... args) //-> decltype(func(std::forward<Args>(args)...))
        {
            auto f = std::bind(func, std::forward<Args>(args)...);
            TimerTask* timerTask = new TimerTask(f);
            time_entry<> tmp_now;
            tmp_now.getTimeOfDay();
            time_entry<> expiration = tmp_now + delayTime;
            add(timerTask, expiration);
        }

        /**
         * addByExpiration的函数说明
         * 
         * @brief 这个函数用于向定时器加入定时任务，如果约定时间到，则执行该任务。
         * @param func 参数模板，接收任意函数名
         * @param args 可变模板参数，接收待传入func函数的所有参数
         * @param expiration 定时时间，类型是TimerExpiration
         *
        */
        template<typename Func, typename ... Args>
        void addByExpiration(TimerExpiration& expiration, const Func& func, Args&& ... args) {
            auto f = std::bind(func, std::forward<Args>(args)...);
            TimerTask* timerTask = new TimerTask(f);
            time_entry<> _expiration = expiration.getTimeEntry();
            add(timerTask, _expiration);
        }


    private:
        std::unique_ptr< Ticker > m_ticker;
        std::unique_ptr<thread_pool::ThreadPool> m_threadPool;
        std::shared_ptr< delay_queue::DelayQueue<TimerTaskList> > m_delayQueue;
        std::unique_ptr<TimingWheel> m_timingWheel;
        std::shared_ptr<std::atomic<int> > m_taskCounter;

        time_entry<> m_tickMs;
        time_entry<> m_startMs;
        int m_wheelSize;
        int m_tickerNum;
        mutable std::shared_mutex m_rwMutex;

        void add(TimerTask* timerTask, time_entry<>);

    };

}
