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

#ifndef SHAREMIND_COUNTMAXACTOR_H
#define SHAREMIND_COUNTMAXACTOR_H

#include <algorithm>
#include <boost/static_assert.hpp>
#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
#include "FunctionTraits.h"


namespace sharemind {

template <typename Actor>
class CountMaxActor {

public: /* Types: */

    using Exception = typename std::remove_reference<Actor>::type::Exception;

private: /* Types: */

    using WantDataType =
        typename std::remove_pointer<
            typename sharemind::FunctionTraits<Actor>
                    ::template argument<1u>::type>::type;

public: /* Methods: */

    CountMaxActor(const CountMaxActor<Actor> &) = delete;
    CountMaxActor<Actor> & operator=(const CountMaxActor<Actor> &) = delete;
    CountMaxActor(CountMaxActor<Actor> &&) = default;
    CountMaxActor<Actor> & operator=(CountMaxActor<Actor> &&) = default;

    template <typename ... Args>
    inline CountMaxActor(Args && ... args)
            noexcept(noexcept(Actor(std::forward<Args>(args)...)))
        : m_actor(std::forward<Args>(args)...)
        , m_count(0u) {}

    inline size_t operator()(WantDataType * const data, const size_t size)
            noexcept(noexcept(std::declval<Actor &>()(data, size)))
    {
        if (data) {
            assert(size > 0u);
            const size_t maxTransfer =
                    std::numeric_limits<size_t>::max() - m_count;
            if (maxTransfer == 0u)
                return 0u;
            const size_t canTransfer = std::min(size, maxTransfer);
            const size_t transferred = m_actor(data, canTransfer);
            assert(transferred <= canTransfer);
            m_count += transferred;
            return transferred;
        } else {
            return m_actor(data, size);
        }
    }

    inline size_t count() const noexcept { return m_count; }

private: /* Fields: */

    Actor m_actor;
    size_t m_count;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_COUNTMAXACTOR_H */
