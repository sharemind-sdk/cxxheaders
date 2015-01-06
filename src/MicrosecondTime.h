/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#ifndef SHAREMIND_MICROSECONDTIMER_H
#define SHAREMIND_MICROSECONDTIMER_H

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

#endif /* SHAREMIND_MICROSECONDTIMER_H */
