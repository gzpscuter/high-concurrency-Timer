/**
 *
 *  Ticker.h
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#pragma once

#include <memory>
#include <sys/select.h>
#include <sys/time.h>

#include "Timer.h"
#include "delay_queue/TimeEntry.h"

namespace timer {

    class Timer;
    class Ticker {
    public:
        /// Ticker构造函数说明
        ///　       
        ///　       该构造函数用于构造Ticker对象，是定时器对象
        ///         @param tickerInterval 类型long，是定时器轮询的时间间隔，单位是ms
        ///         @param timer 类型Timer*，指向包含该Ticker的Timer类对象地址
        Ticker(time_entry<> tickInterval, Timer* timer);
        ~Ticker();
        void startTick();
        void stop();

    private:
        time_entry<> m_timeout;
        Timer* m_timerObj;
        volatile bool m_startFlag = true;

    };
}