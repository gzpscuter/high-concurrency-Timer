/**
 *
 *  main.cc
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
#include <deque>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <assert.h>

#include "NonCopyable.h"

namespace thread_pool {
  
    class ThreadPool : public NonCopyable{
    public:
        typedef std::function<void()> Task;

        /**
         * ThreadPool类构造函数说明
         * 
         * @param init_size 初始化线程池的线程数目
        */
        explicit ThreadPool(int init_size);
        ~ThreadPool();

        /// stop接口的说明   
        ///     
        ///      该接口将使线程池进入关闭状态，停止接收新任务，并在执行完毕所有任务队列的任务后析构线程池对象     
        void stop();

        /// addTask接口的说明   
        ///     
        ///      该接口可以供外部添加新任务到线程池任务队列中     
        ///      @param Ｔask& 添加的任务，格式为void()         
        void addTask(const Task&);

    private:
        typedef std::vector<std::thread*> threads_t;
        typedef std::deque<Task> tasks_t;

        /// 线程池执行状态，true->执行，false->关闭
        bool m_isStarted;

        /// 初始化线程池线程数组数目
        int m_initSize;

        ///　线程数组
        threads_t m_threads;

        ///　任务队列
        tasks_t m_tasks;
        
        /// 进行同步互斥的互斥量和条件变量
        std::mutex m_mutex;
        std::condition_variable m_cond;

        void start();
        void threadLoop();
        Task take();

        inline const bool isStarted() {return m_isStarted;}
    };
}