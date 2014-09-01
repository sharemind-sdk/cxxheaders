/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_COMMON_MICROSECONDTIMER_H
#define SHAREMIND_COMMON_MICROSECONDTIMER_H

#ifdef __cplusplus
#include <cassert>
#include <cstddef>
#include <cstdint>
#else
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#endif
#include <sys/time.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef uint_fast64_t SharemindUsTime;

/** \returns the current "time" in microseconds. */
inline SharemindUsTime sharemindGetUsTime(const SharemindUsTime onFail = 0u) {
    timeval t;
    if (gettimeofday(&t, nullptr) != 0)
        return onFail;
    assert(t.tv_sec >= 0);
    assert(t.tv_usec >= 0);
    assert(t.tv_usec < 1000000);
    #ifdef __cplusplus
    return static_cast<uint64_t>(t.tv_sec) * 1000000u
           + static_cast<uint64_t>(t.tv_usec);
    #else
    return ((uint64_t) t.tv_sec) * 1000000u + ((uint64_t) t.tv_usec);
    #endif
}

#ifdef __cplusplus
} /* extern "C" { */

namespace sharemind {

typedef SharemindUsTime UsTime;

/** \returns the current "time" in microseconds. */
inline SharemindUsTime getUsTime(const SharemindUsTime onFail = 0u) noexcept
{ return ::sharemindGetUsTime(onFail); }

} /* namespace sharemind { */

#endif

#endif /* SHAREMIND_COMMON_MICROSECONDTIMER_H */
