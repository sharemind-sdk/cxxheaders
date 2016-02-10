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
#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
#include "FunctionTraits.h"
#include "PartialStreamOperationException.h"


namespace sharemind {

template <typename Actor, bool COUNT_PARTIAL = false>
class CountMaxActor {

public: /* Types: */

    using Exception = typename std::remove_reference<Actor>::type::Exception;

private: /* Types: */

    using WantDataType =
        typename std::remove_pointer<
            typename sharemind::FunctionTraits<Actor>
                    ::template argument<1u>::type>::type;

public: /* Methods: */

    CountMaxActor(CountMaxActor<Actor, COUNT_PARTIAL> const &) = delete;
    CountMaxActor<Actor> & operator=(
            CountMaxActor<Actor, COUNT_PARTIAL> const &) = delete;
    CountMaxActor(CountMaxActor<Actor, COUNT_PARTIAL> &&) = default;
    CountMaxActor<Actor> & operator=(
            CountMaxActor<Actor, COUNT_PARTIAL> &&) = default;

    template <typename ... Args>
    inline CountMaxActor(Args && ... args)
            noexcept(noexcept(Actor(std::forward<Args>(args)...)))
        : m_actor(std::forward<Args>(args)...)
        , m_count(0u) {}

    inline std::size_t operator()(WantDataType * const data,
                                  std::size_t const size)
            noexcept(noexcept(std::declval<Actor &>()(data, size)))
    {
        assert(data);
        assert(size > 0u);
        std::size_t const maxTransfer =
                std::numeric_limits<std::size_t>::max() - m_count;
        if (maxTransfer == 0u)
            return 0u;
        std::size_t const canTransfer = std::min(size, maxTransfer);
        try {
            std::size_t const transferred = m_actor(data, canTransfer);
            assert(transferred <= canTransfer);
            m_count += transferred;
            return transferred;
        } catch (PartialStreamOperationException const & e) {
            assert(e.size() < canTransfer);
            if (COUNT_PARTIAL)
                m_count += e.size();
            throw;
        } catch (Exception &) {
            throw;
        } catch (...) {
            std::unexpected();
        }
    }

    inline std::size_t count() const noexcept { return m_count; }

private: /* Fields: */

    Actor m_actor;
    std::size_t m_count;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_COUNTMAXACTOR_H */
