/*
 * Copyright (C) Cybernetica
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

template <typename CHRONO_ = std::chrono::milliseconds>
class LoopDuration: public CHRONO_ {

public: /* Methods: */

    LoopDuration(std::size_t const ms) noexcept(noexcept(CHRONO_(ms)))
        : CHRONO_(ms)
    {}

};

template <std::size_t MS_ = 3u, typename CHRONO_ = std::chrono::milliseconds>
class StaticLoopDuration: public LoopDuration<CHRONO_> {

public: /* Methods: */

    StaticLoopDuration() noexcept(noexcept(LoopDuration<CHRONO_>(MS_)))
        : LoopDuration<CHRONO_>(MS_)
    {}

};

} /* namespace sharemind { */

#endif /* SHAREMIND_DURATIONS_H */
