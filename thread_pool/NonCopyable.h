/**
 *
 *  NonCopyable.h
 *  gzp-amateur
 *
 *  Copyright 2020, gzp-amateur.  All rights reserved.
 *  https://github.com/gzpscuter/reposit/Timer
 *
 *  Timer
 *
 */

#pragma once

namespace thread_pool {
    class NonCopyable {
    protected:
        NonCopyable() {};
        ~NonCopyable() {};

    private:
        NonCopyable(const NonCopyable&);
        NonCopyable& operator=(const NonCopyable&);
    };
}
