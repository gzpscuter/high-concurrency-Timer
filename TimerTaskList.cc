/**
 *
 *  TimerTaskList.cc
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#include "TimerTaskList.h"

using namespace timer;

TimerTaskList::TimerTaskList()
{}

TimerTaskList::TimerTaskList(std::shared_ptr< std::atomic<int> > taskCounter)
:m_taskCounter(taskCounter),
m_mtx(new std::recursive_mutex),
m_setFlag(new std::atomic_flag(ATOMIC_FLAG_INIT)),
m_expiration(time_entry<>()),
root(new TimerTaskEntry(nullptr, time_entry<>()))
{
    root->next = root.get();
    root->prev = root.get();
}

TimerTaskList::TimerTaskList(const TimerTaskList& timerTaskList)
{
    m_mtx = timerTaskList.m_mtx;
    root = timerTaskList.root;
    root->next = timerTaskList.root->next;
    root->prev = timerTaskList.root->prev;
    m_taskCounter = timerTaskList.m_taskCounter;
    m_setFlag = timerTaskList.m_setFlag;
    m_expiration = timerTaskList.m_expiration;
}

TimerTaskList& TimerTaskList::operator=(const TimerTaskList& timerTaskList)
{
    m_mtx = timerTaskList.m_mtx;
    root = timerTaskList.root;
    m_taskCounter = timerTaskList.m_taskCounter;
    m_setFlag = timerTaskList.m_setFlag;
    m_expiration = timerTaskList.m_expiration;
}

void TimerTaskList::add(TimerTaskEntry* timerTaskEntry)
{
    timerTaskEntry->remove();
    std::unique_lock<std::recursive_mutex> lock(*m_mtx);
    if(timerTaskEntry->list == nullptr) {
        TimerTaskEntry* tail = root->prev;
        timerTaskEntry->next = root.get();
        timerTaskEntry->prev = tail;
        timerTaskEntry->list = this;
        tail->next = timerTaskEntry;
        root->prev = timerTaskEntry;
        (*m_taskCounter)++;
    }
}

void TimerTaskList::remove(TimerTaskEntry* timerTaskEntry)
{
    std::unique_lock<std::recursive_mutex> lock(*m_mtx);
    if(timerTaskEntry->list == this) {
        timerTaskEntry->next->prev = timerTaskEntry->prev;
        timerTaskEntry->prev->next = timerTaskEntry->next;
        timerTaskEntry->next = nullptr;
        timerTaskEntry->prev = nullptr;
        timerTaskEntry->list = nullptr;
        (*m_taskCounter)--;
    }
}

void TimerTaskList::flush(Timer* callBackTimer)
{
    std::unique_lock<std::recursive_mutex> lock(*m_mtx);
    // std::bind must have an instatnce as the second parameter, when used in calling a nonstatic class member function
    auto f = std::bind(&Timer::addTimerTaskEntry, callBackTimer, std::placeholders::_1);
    TimerTaskEntry* head = root->next;
    while(head != nullptr && head != root.get()) {
        remove(head);
        f(head);
        head = root->next;
    }
    while(!m_setFlag->test_and_set()) {
        m_expiration = {-1L, -1L};
        m_setFlag->clear();
        break;
    }
}

bool TimerTaskList::setExpiration(time_entry<> expiration)
{
    while(!m_setFlag->test_and_set()) {
        time_entry<> oriExpiration = m_expiration;
        m_expiration = expiration;
        m_setFlag->clear();
        return expiration != oriExpiration;
    }
}

const time_entry<> TimerTaskList::getExpiration()
{
    while(!m_setFlag->test_and_set()) {
        time_entry<> expiration = m_expiration;
         m_setFlag->clear();
        return expiration;
    }
}


TimerTaskEntry::TimerTaskEntry(TimerTask* timerTask, time_entry<> tv_expriation)
:m_expiration(tv_expriation),
m_timerTask(timerTask)
{
    if(timerTask != nullptr)
        timerTask->setTimerTaskEntry(this);
}

TimerTaskEntry::TimerTaskEntry(const TimerTaskEntry& timerTaskEntry)
{
    m_expiration = timerTaskEntry.m_expiration;
    m_timerTask = timerTaskEntry.m_timerTask;
    list = timerTaskEntry.list;
    prev = timerTaskEntry.prev;
    next = timerTaskEntry.next;
}

void TimerTaskEntry::remove()
{
    TimerTaskList* currentList = list;
    while(currentList != nullptr) {
        currentList->remove(this);
        currentList = list;
    }
}

bool TimerTaskEntry::cancelled()
{
    return m_timerTask->getTimerTaskEntry() != this; 
}

const time_entry<> TimerTaskEntry::getExpiration()
{
    return m_expiration;
}

TimerTask* TimerTaskEntry::getTimerTask()
{
    return m_timerTask;
}


void TimerTask::setTimerTaskEntry(TimerTaskEntry* entry)
{
    if(m_timerTaskEntry != nullptr && m_timerTaskEntry != entry)
        m_timerTaskEntry->remove();

    m_timerTaskEntry = entry;
}

TimerTaskEntry* TimerTask::getTimerTaskEntry()
{
    return m_timerTaskEntry;
}
