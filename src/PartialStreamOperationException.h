/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_PARTIALSTREAMOPERATIONEXCEPTION_H
#define SHAREMIND_PARTIALSTREAMOPERATIONEXCEPTION_H

#include "Exception.h"

#include <cassert>
#include <limits>


namespace sharemind {

class PartialStreamOperationException: public Exception {

public: /* Methods: */

    inline PartialStreamOperationException(size_t const doneSize) noexcept
        : m_doneSize((assert(doneSize > 0u), doneSize)) {}

    inline const char * what() const noexcept final override
    { return "Partial operation"; }

    void addToSize(size_t const moreDoneSize) noexcept {
        assert(std::numeric_limits<size_t>::max() - m_doneSize >= moreDoneSize);
        m_doneSize += moreDoneSize;
    }

    inline size_t size() const noexcept
    { return (assert(m_doneSize > 0u), m_doneSize); }

    [[noreturn]] inline void rethrowInner() const {
        try {
            std::rethrow_if_nested(*this);
            std::unexpected();
        } catch (PartialStreamOperationException const &) {
            std::unexpected();
        }
    }

    [[noreturn]] static inline void throwWithCurrent(size_t const doneSize) {
        if (doneSize > 0u)
            std::throw_with_nested(PartialStreamOperationException{doneSize});
        throw;
    }

private: /* Types: */

    size_t m_doneSize;

};

} /* namespace sharemind {*/

#endif /* SHAREMIND_PARTIALSTREAMOPERATIONEXCEPTION_H */
