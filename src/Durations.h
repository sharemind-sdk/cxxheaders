/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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
