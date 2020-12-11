/**
 *
 *  TimerTaskList.h
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#pragma once

#include <functional>
#include <sys/time.h>
#include <mutex>
#include <atomic>
#include <memory>

#include "delay_queue/TimeEntry.h"
#include "Timer.h"

#include <iostream>

namespace timer {
    class Timer;


    class TimerTask;
    class TimerTaskEntry;
    class TimerTaskList;

    class TimerTask {

        TimerTaskEntry* m_timerTaskEntry;
        std::function<void()> m_func;

    public:
        template<typename Func>
        explicit TimerTask(Func f)
        :m_timerTaskEntry(nullptr),
         m_func(f)
         {}

        std::function<void()> getFunc()
        {
            return m_func;
        }

        void setTimerTaskEntry(TimerTaskEntry* entry);

        TimerTaskEntry* getTimerTaskEntry();
    };


    class TimerTaskEntry {
        TimerTask* m_timerTask;
        time_entry<> m_expiration;

    public:
        TimerTaskEntry(TimerTask* timerTask, time_entry<> tv_expriation);
        TimerTaskEntry(const TimerTaskEntry& timerTaskEntry);
        ~TimerTaskEntry() {
            if(nullptr != prev) {
                prev = nullptr;
            } 
            if(nullptr != next) {
                next = nullptr;
            }
            if(nullptr != m_timerTask){
                delete m_timerTask;
                m_timerTask = nullptr;
            } 
        }

        TimerTaskList* list = nullptr;
        TimerTaskEntry* prev = nullptr;
        TimerTaskEntry* next = nullptr;

        bool cancelled();
        void remove();
        const time_entry<> getExpiration();
        TimerTask* getTimerTask();
    };


    class TimerTaskList {
        /**
         * std::mutex description
         *
         * @brief std::mutex, std::atomic_flag cannot be constructed through copy-constructor method, we need
         *          pointer and new-operation to confirm each instance has its own mutex. Here, shared_ptr is
         *          used to avoid mis-deletion of allocated space of mutex and atomic_flag.
         * */
        std::shared_ptr<std::recursive_mutex> m_mtx;
        std::shared_ptr< TimerTaskEntry > root;
        std::shared_ptr< std::atomic<int> > m_taskCounter;
        std::shared_ptr< std::atomic_flag > m_setFlag; 
        time_entry<> m_expiration;

    public:
        TimerTaskList(std::shared_ptr< std::atomic<int> > taskCounter);
        /// for vector push_back, we need copy-constructor
        TimerTaskList();
        TimerTaskList(const TimerTaskList& timerTaskList);
        TimerTaskList& operator=(const TimerTaskList& timerTaskList);
        ~TimerTaskList() {
            auto itd = root->next, itn = itd;
            while(nullptr != itd->next && itd != root.get()) {
                itn = itd->next;
                delete itd;
                itd = itn;
            }
        }

        void add(TimerTaskEntry* timerTaskEntry);
        void remove(TimerTaskEntry* timerTaskEntry);
        void flush(Timer* callBackTimer);
        bool setExpiration(time_entry<> expiration);
        const time_entry<> getExpiration();
    };

}