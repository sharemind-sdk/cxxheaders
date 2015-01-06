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

#ifndef SHAREMIND_DURATIONS_H
#define SHAREMIND_DURATIONS_H

#include <chrono>
#include <cstddef>


namespace sharemind {

template <typename CHRONO__ = std::chrono::milliseconds>
class LoopDuration: public CHRONO__ {

public: /* Methods: */

    inline LoopDuration(const size_t ms)
            noexcept(noexcept(CHRONO__(ms)))
        : CHRONO__(ms) {}

};

template <size_t MS__ = 3u, typename CHRONO__ = std::chrono::milliseconds>
class StaticLoopDuration: public LoopDuration<CHRONO__> {

public: /* Methods: */

    inline StaticLoopDuration()
            noexcept(noexcept(LoopDuration<CHRONO__>(MS__)))
        : LoopDuration<CHRONO__>(MS__) {}

};

} /* namespace sharemind { */

#endif /* SHAREMIND_DURATIONS_H */
