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

#ifndef SHAREMIND_IDENTIFIERPOOL_H
#define SHAREMIND_IDENTIFIERPOOL_H

#include <cassert>
#include <stdexcept>
#include <set>
#include <type_traits>
#include "Exception.h"


namespace sharemind {

template <typename T>
class IdentifierPool {

    static_assert(std::is_unsigned<T>::value,  "T must be unsigned!");
    static_assert(!std::is_const<T>::value,    "T must not be const!");
    static_assert(!std::is_volatile<T>::value, "T must not be volatile!");

public: /* Types: */

    SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);
    /**
      \brief Signifies that ID reservation failed because the identifier space
             in type T is exhausted.
    */
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            ReserveException,
            "No more identifiers can be reserved.");

public: /* Methods: */

    /**
      \brief Generates, reserves and returns an unique ID from this pool.
      \returns the unique ID generated and reserved.
      \throws ReserveException on pool exhaustion.
      \throws std::bad_alloc when out of memory.
    */
    T reserve() {
        T tryNext = m_tryNextId;
        T const oldTryNext = tryNext;
        while (m_reserved.find(tryNext) != m_reserved.end())
            if (++tryNext == oldTryNext)
                throw ReserveException();
        m_reserved.insert(tryNext);
        m_tryNextId = tryNext + 1u;
    }

    /**
      \brief Returns whether the given ID is reserved.
      \param[in] id The ID to check.
      \returns whether the given ID is reserved.
    */
    inline bool reserved(T const id) const noexcept
    { return m_reserved.find(id) != m_reserved.end(); }

    /**
       \brief Releases the given ID back to the pool.
       \param[in] id The ID to release.
    */
    inline void recycle(T const id) noexcept {
        assert(m_reserved.find(id) != m_reserved.end());
        m_reserved.erase(id);
    }

private: /* Fields: */

    std::set<T> m_reserved; /**< The ordered set of reserved IDs. */
    T m_tryNextId = 1u;     /**< The next ID to try to reserve. */

};

} /* namespace sharemind { */

#endif /* SHAREMIND_IDENTIFIERPOOL_H */
