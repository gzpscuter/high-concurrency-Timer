/**
 *
 *  TimingWheel.cc
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#include "TimingWheel.h"
#include <iostream>

using namespace timer;

TimingWheel::TimingWheel(time_entry<> tickMs, int wheelSize, time_entry<> startMs, std::shared_ptr< std::atomic<int> > taskCounter, std::shared_ptr< delay_queue::DelayQueue<TimerTaskList> > queue)
:m_tickMs(tickMs),
m_wheelSize(wheelSize),
m_taskCounter(taskCounter),
m_queue(queue),
m_overflowWheel(nullptr)
{
    m_buckets.resize(wheelSize, TimerTaskList(taskCounter));
    m_interval = tickMs * wheelSize;
    m_currentTime = startMs - (startMs % tickMs);
}

TimingWheel::~TimingWheel()
{
    if(nullptr != m_overflowWheel)
        delete m_overflowWheel;
}

void TimingWheel::addOverflowWheel()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_overflowWheel == nullptr)
        m_overflowWheel = new TimingWheel(
            m_interval,
            m_wheelSize,
            m_currentTime,
            m_taskCounter,
            m_queue
        );
}

bool TimingWheel::add(TimerTaskEntry* timerTaskEntry)
{
    time_entry<> expiration = timerTaskEntry->getExpiration();
    
    if(timerTaskEntry->cancelled())
        return false;
    else if(expiration < (m_currentTime + m_tickMs))
        return false;
    else if (expiration < (m_currentTime + m_interval)) {
        long virtualId = expiration / m_tickMs;
        TimerTaskList& bucket = m_buckets.at(virtualId % m_wheelSize);
        bucket.add(timerTaskEntry);

        if(bucket.setExpiration(m_tickMs * virtualId)) {
            m_queue->offer(&bucket, expiration);
        }
        return true;
    }
    else {
        if(nullptr == m_overflowWheel)  {
            addOverflowWheel();
        }
        m_overflowWheel->add(timerTaskEntry);
    }
}

void TimingWheel::advanceClock(time_entry<> timeMs)
{
    if(timeMs >= (m_currentTime + m_tickMs)) {
        m_currentTime = timeMs - (timeMs % m_tickMs);

        if(m_overflowWheel != nullptr) m_overflowWheel->advanceClock(m_currentTime);
    }
}