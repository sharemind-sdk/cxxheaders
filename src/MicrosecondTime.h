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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sys/time.h>
#include <type_traits>


namespace sharemind {

using UsTime = uint_fast64_t;

/** \returns the current "time" in microseconds. */
inline UsTime getUsTime(UsTime const onFail = 0u) noexcept {
    timeval t;
    if (gettimeofday(&t, nullptr) != 0)
        return onFail;
    assert(t.tv_sec >= 0);
    assert(t.tv_usec >= 0);
    assert(t.tv_usec < 1000000);
    using Sec = typename std::make_unsigned<decltype(t.tv_sec)>::type;
    using Usec = typename std::make_unsigned<decltype(t.tv_usec)>::type;
    return static_cast<Sec>(t.tv_sec) * 1000000u + static_cast<Usec>(t.tv_usec);
}

/** \returns the current "time" in microseconds. */
template <typename E>
inline UsTime getUsTimeOrThrow(E e) {
    timeval t;
    if (gettimeofday(&t, nullptr) != 0)
        throw e;
    assert(t.tv_sec >= 0);
    assert(t.tv_usec >= 0);
    assert(t.tv_usec < 1000000);
    using Sec = typename std::make_unsigned<decltype(t.tv_sec)>::type;
    using Usec = typename std::make_unsigned<decltype(t.tv_usec)>::type;
    return static_cast<Sec>(t.tv_sec) * 1000000u + static_cast<Usec>(t.tv_usec);
}

} /* namespace sharemind { */

#endif /* SHAREMIND_MICROSECONDTIMER_H */
