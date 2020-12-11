/**
 *
 *  Timer.cc
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#include "Timer.h"
#include <iostream>

using namespace timer;

Timer::Timer(time_entry<> tickMs, int wheelSize, int tickerNum, time_entry<> timeout , int threadNum )
:m_tickMs(tickMs),
m_wheelSize(wheelSize),
m_tickerNum(tickerNum),
m_taskCounter(new std::atomic<int>(0)),
m_ticker(new Ticker(timeout,this)),
m_threadPool(new thread_pool::ThreadPool(threadNum)),
m_delayQueue(new delay_queue::DelayQueue<TimerTaskList>())
{
    m_startMs.getTimeOfDay();
    m_timingWheel = make_unique<TimingWheel>(m_tickMs, m_wheelSize, m_startMs, m_taskCounter, m_delayQueue);
}

void Timer::start()
{
    std::cout << "计时器启动" << endl;
    for (int i = 0; i < m_tickerNum; i++)
        m_threadPool->addTask(std::bind(&Ticker::startTick, m_ticker.get()));
}

void Timer::add(TimerTask* timerTask, time_entry<> expiration)
{
    std::shared_lock<std::shared_mutex> lock(m_rwMutex);
    addTimerTaskEntry(new TimerTaskEntry(timerTask, expiration));
}

void Timer::addTimerTaskEntry(TimerTaskEntry* timerTaskEntry)
{
    if(!m_timingWheel->add(timerTaskEntry)) {
        if(!timerTaskEntry->cancelled()) {
            TimerTask* timerTask = timerTaskEntry->getTimerTask();
            m_threadPool->addTask(timerTask->getFunc());
            delete timerTaskEntry;
        }
            
    }
}

void Timer::advanceClock()
{
    auto bucket = m_delayQueue->poll();
    if(bucket != nullptr) {
        std::unique_lock<std::shared_mutex> lock(m_rwMutex);
        while(bucket != nullptr) {
            m_timingWheel->advanceClock(bucket->getExpiration());
            bucket->flush(this);
            bucket = m_delayQueue->poll();
        }
    }
}

void Timer::shutdown()
{
    m_ticker->stop();
    m_threadPool->stop();
    std::cout<< "Timer has been stopped!";
}

Timer::~Timer()
{
    m_ticker.reset();
    m_threadPool.reset();
    m_timingWheel.reset();
    m_delayQueue.reset();

}