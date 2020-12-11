/**
 *
 *  ThreadPool.cc
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#include "ThreadPool.h"

namespace thread_pool {
    ThreadPool::ThreadPool(int initSize)
    :m_initSize(initSize), m_isStarted(false)
    {
        start();
    }

    ThreadPool::~ThreadPool()
    {
        if(isStarted()) 
            stop();
    }
  
    void ThreadPool::start()
    {
        assert(m_threads.empty());
        m_isStarted = true;
        m_threads.reserve(m_initSize);
        for(int i=0; i<m_initSize; i++) {
            m_threads.emplace_back(new std::thread(std::bind(&ThreadPool::threadLoop, this)));
        }
    }

    void ThreadPool::stop()
    { 
        // std::unique_lock<std::mutex> lock(m_mutex);
        m_isStarted = false;
        m_cond.notify_all();

        for(auto it=m_threads.begin(); it<m_threads.end(); it++) {
            (*it)->join();
            delete *it;
        }
        // while(!m_tasks.empty()){
        //     std::cout<<"m_task size "<<m_tasks.size()<<"  m_isStarted "<<m_isStarted<<std::endl;
        //     continue;
        // }
            
        
        m_threads.clear();
        std::vector<std::thread*>().swap(m_threads);
        std::cout <<"ThreadPool has been stoped" << std::endl;
    }

    void ThreadPool::threadLoop()
    {
        while(m_isStarted || !m_tasks.empty()) {
            Task task = take();
            if(task) {
                task();
            }
        }
        // std::cout << "thread  " << std::this_thread::get_id() << " exit loop" << std::endl;
    }

    void ThreadPool::addTask(const Task& task)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_isStarted ) {
            m_tasks.emplace_back(task);
            m_cond.notify_one();
        }
   
    }

    ThreadPool::Task ThreadPool::take()
    { 
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_tasks.empty() && m_isStarted) {
            m_cond.wait(lock);
        }

        Task task;
        tasks_t::size_type size = m_tasks.size();
        if(!m_tasks.empty()) {
            task = m_tasks.front();
            m_tasks.pop_front();
            assert(size -1 == m_tasks.size());
        }

        return task;
    }
}