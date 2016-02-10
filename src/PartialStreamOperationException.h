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

#ifndef SHAREMIND_PARTIALSTREAMOPERATIONEXCEPTION_H
#define SHAREMIND_PARTIALSTREAMOPERATIONEXCEPTION_H

#include "Exception.h"

#include <cassert>
#include <cstddef>
#include <limits>
#include "compiler-support/GccNoreturn.h"


namespace sharemind {

class PartialStreamOperationException: public Exception {

public: /* Methods: */

    inline PartialStreamOperationException(std::size_t const doneSize) noexcept
        : m_doneSize((assert(doneSize > 0u), doneSize)) {}

    inline char const * what() const noexcept final override
    { return "Partial operation"; }

    void addToSize(std::size_t const moreDoneSize) noexcept {
        assert(std::numeric_limits<std::size_t>::max() - m_doneSize
               >= moreDoneSize);
        m_doneSize += moreDoneSize;
    }

    inline std::size_t size() const noexcept
    { return (assert(m_doneSize > 0u), m_doneSize); }

    SHAREMIND_GCC_NORETURN_PART1
    inline void rethrowInner() const SHAREMIND_GCC_NORETURN_PART2 {
        try {
            std::rethrow_if_nested(*this);
            std::unexpected();
        } catch (PartialStreamOperationException const &) {
            std::unexpected();
        }
    }

    SHAREMIND_GCC_NORETURN_PART1
    static inline void throwWithCurrent(std::size_t const doneSize)
            SHAREMIND_GCC_NORETURN_PART2
    {
        if (doneSize > 0u)
            std::throw_with_nested(PartialStreamOperationException{doneSize});
        throw;
    }

private: /* Types: */

    std::size_t m_doneSize;

};

} /* namespace sharemind {*/

#endif /* SHAREMIND_PARTIALSTREAMOPERATIONEXCEPTION_H */
