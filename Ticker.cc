/**
 *
 *  Ticker.cc
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#include "Ticker.h"

#include <iostream>

using namespace timer;

Ticker::Ticker(time_entry<> tickInterval, Timer* timer)
: m_timeout(tickInterval),
m_timerObj(timer)
{
}

Ticker::~Ticker()
{
    if(nullptr != m_timerObj)
        m_timerObj = nullptr;
}

void Ticker::stop()
{
    m_startFlag = false;
    std::cout << "Ticker has been stopped..." << endl;
}

void Ticker::startTick()
{
    while(m_startFlag) {
        timeval timeout = m_timeout.getTimeval();
        select(1, NULL, NULL, NULL, &timeout);
        m_timerObj->advanceClock();
    }
}
