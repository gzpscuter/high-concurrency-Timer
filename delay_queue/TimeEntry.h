/**
 *
 *  TimeEntry.h
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#pragma once

#include <sys/time.h>
#include <stdarg.h>
#include <vector>
#include <algorithm>

using namespace std;

/// timeval_entry的结构体说明
///
/// @brief 这个结构体目前是对timeval的封装，方便实现timeval运算符重载，加入模板是为了
///        对time_t或者tm的封装的尝试，目前只支持timeval，生而为人我很抱歉
template<typename T = timeval>
struct time_entry {
    typedef time_entry<T>    _self;
    typedef T                   value_type;
    value_type __node;

    time_entry(void) 
    {
        this->__node = {-1L, -1L};
    }

    time_entry(const _self& val) {
        this->__node = val.__node;
    }

    time_entry(const value_type& val) noexcept{
        __node = val;
    }

    _self& operator=(const _self& val) {
        if(this == &val) return *this;
        this->__node = val.__node;
        return *this;
    }

    _self& operator=(const value_type& val) {
        this->__node = val;
        return *this;
    }

    friend
    _self operator-(const _self& lval, const _self& rval) {
        long sum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec - rval.__node.tv_sec*1000000 - rval.__node.tv_usec;
        if(sum < 0)
            return _self({-1L, -1L});
        value_type ans = {sum/1000000, sum%1000000};
        _self time_ans(ans);
        return time_ans;         
    }

    friend
    const _self operator+(const _self& lval, const _self& rval) {
        long sum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec + rval.__node.tv_sec*1000000 + rval.__node.tv_usec;
        value_type ans = {sum/1000000, sum%1000000};
        _self time_ans(ans);
        return time_ans;           
    }

    friend
    const _self operator%(const _self& lval, const _self& rval) {
        long lsum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec;
        long rsum = rval.__node.tv_sec*1000000 + rval.__node.tv_usec;
        value_type ans = {lsum % rsum / 1000000, lsum % rsum % 1000000};
        _self time_ans(ans);
        return time_ans;
    }

    friend
    const _self operator*(const _self& lval, const long& rval) {
        long lsum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec;
        value_type ans = {lsum * rval / 1000000, lsum * rval % 1000000};
        _self time_ans(ans);
        return time_ans;
    }

    friend
    const long operator/(const _self& lval, const _self& rval) {
        long lsum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec;
        long rsum = rval.__node.tv_sec*1000000 + rval.__node.tv_usec;
        return lsum / rsum;
    }

    friend
    bool operator<(const _self& lval, const _self& rval) {
        long lsum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec;
        long rsum = rval.__node.tv_sec*1000000 + rval.__node.tv_usec;
        return lsum < rsum;
    }

    friend
    bool operator>(const _self& lval, const _self& rval) {
        long lsum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec;
        long rsum = rval.__node.tv_sec*1000000 + rval.__node.tv_usec;
        return lsum > rsum;
    }

    friend
    bool operator>=(const _self& lval, const _self& rval) {
        long lsum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec;
        long rsum = rval.__node.tv_sec*1000000 + rval.__node.tv_usec;
        return lsum >= rsum;
    }

    friend
    bool operator!=(const _self& lval, const _self& rval) {
        long lsum = lval.__node.tv_sec*1000000 + lval.__node.tv_usec;
        long rsum = rval.__node.tv_sec*1000000 + rval.__node.tv_usec;
        return lsum != rsum;
    }

    friend
    ostream & operator<<( ostream & os,const _self & val) {
        os << val.__node.tv_sec << "s " << val.__node.tv_usec << "us";
        return os;
    }

    value_type getTimeval() {
        return this->__node;
    }

    void getTimeOfDay() {
        gettimeofday(&this->__node, NULL);
    }

};

/**
 * Description of TimerExpiration
 *
 * @brief the TimerExpiration structor is utilized to set the expiration time of the timing task
 * @param args args is variable parameter template, but you have to feed parameters in a way formatting like
 * year, month, month-day, hour, minute, second, microsecond. Note that any unset parameter will
 * be set to 0 by default.
 * */
struct TimerExpiration
{
    typedef time_entry<>    val_type;

    vector<long> m_timevars;

    TimerExpiration(void) = default ;

    TimerExpiration(const TimerExpiration& timerExpiration) {
        this->m_timevars = timerExpiration.m_timevars;
    }

    template<class T>
    void setTime(T&& t) {
        m_timevars.push_back(t);
    }

    template<typename T, typename ... Args>
    void setTime(T&& head, Args&& ... args) {
        m_timevars.push_back(head);
        setTime(args...);
    }

    template<typename ... Args>
    TimerExpiration(Args&& ... args) {
        setTime(std::forward<Args>(args)...);
    }


    const timeval getTimeval() {
        vector<long> tmp_timevars(7,0);
        int i = 0;
        for_each(begin(m_timevars), end(m_timevars), [&](long& x) {
            tmp_timevars[i] = x;
            i++;
        });

        struct tm tmpTm = {
                static_cast<int>(tmp_timevars[5]),
                static_cast<int>(tmp_timevars[4]),
                static_cast<int>(tmp_timevars[3]),
                static_cast<int>(tmp_timevars[2]),
                static_cast<int>(tmp_timevars[1]) - 1,
                static_cast<int>(tmp_timevars[0]) - 1900
        };

        time_t time_sec = mktime(&tmpTm);
        return timeval({time_sec, tmp_timevars[6]});
    }
    
    const val_type getTimeEntry() {
        timeval tmp_tv = getTimeval();
        return time_entry<>(tmp_tv);
    }

};