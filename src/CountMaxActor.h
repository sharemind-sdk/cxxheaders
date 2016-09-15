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
namespace Detail {

template <typename Actor>
using ActorWantDataType =
        typename std::remove_pointer<
            typename sharemind::FunctionTraits<Actor>
                    ::template argument<1u>::type>::type;

template <typename Actor,
          bool COUNT_PARTIAL = false,
          bool isNoexcept_ =
                noexcept(std::declval<Actor &>()(
                            std::declval<ActorWantDataType<Actor> * const>(),
                            std::declval<std::size_t const>()))>
class CountMaxActorImpl {

public: /* Constants: */

    static constexpr bool const isNoexcept = false;

public: /* Types: */

    using Exception = typename std::remove_reference<Actor>::type::Exception;

public: /* Methods: */

    CountMaxActorImpl(
            CountMaxActorImpl<Actor, COUNT_PARTIAL, isNoexcept_> const &)
            = delete;
    CountMaxActorImpl(CountMaxActorImpl<Actor, COUNT_PARTIAL, isNoexcept_> &&)
            = default;
    CountMaxActorImpl<Actor, COUNT_PARTIAL, isNoexcept_> & operator=(
            CountMaxActorImpl<Actor, COUNT_PARTIAL, isNoexcept_> &&) = default;
    CountMaxActorImpl<Actor, COUNT_PARTIAL, isNoexcept_> & operator=(
            CountMaxActorImpl<Actor, COUNT_PARTIAL, isNoexcept_> const &)
            = delete;

    template <typename ... Args>
    inline CountMaxActorImpl(Args && ... args)
            noexcept(noexcept(Actor(std::forward<Args>(args)...)))
        : m_actor(std::forward<Args>(args)...)
    {}

    inline std::size_t operator()(ActorWantDataType<Actor> * const data,
                                  std::size_t const size)
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
    std::size_t m_count{0u};

};

template <typename Actor, bool COUNT_PARTIAL>
class CountMaxActorImpl<Actor, COUNT_PARTIAL, true> {

public: /* Constants: */

    static constexpr bool const isNoexcept = true;

public: /* Types: */

    using Exception = typename std::remove_reference<Actor>::type::Exception;

public: /* Methods: */

    CountMaxActorImpl(CountMaxActorImpl<Actor, COUNT_PARTIAL, true> &&)
            = default;
    CountMaxActorImpl(CountMaxActorImpl<Actor, COUNT_PARTIAL, true> const &)
            = delete;
    CountMaxActorImpl<Actor, COUNT_PARTIAL, true> & operator=(
            CountMaxActorImpl<Actor, COUNT_PARTIAL, true> &&) = default;
    CountMaxActorImpl<Actor, COUNT_PARTIAL, true> & operator=(
            CountMaxActorImpl<Actor, COUNT_PARTIAL, true> const &) = delete;

    template <typename ... Args>
    inline CountMaxActorImpl(Args && ... args)
            noexcept(noexcept(Actor(std::forward<Args>(args)...)))
        : m_actor(std::forward<Args>(args)...)
    {}

    inline std::size_t operator()(ActorWantDataType<Actor> * const data,
                                  std::size_t const size) noexcept
    {
        assert(data);
        assert(size > 0u);
        std::size_t const maxTransfer =
                std::numeric_limits<std::size_t>::max() - m_count;
        if (maxTransfer == 0u)
            return 0u;
        std::size_t const canTransfer = std::min(size, maxTransfer);
        std::size_t const transferred = m_actor(data, canTransfer);
        assert(transferred <= canTransfer);
        m_count += transferred;
        return transferred;
    }

    inline std::size_t count() const noexcept { return m_count; }

private: /* Fields: */

    Actor m_actor;
    std::size_t m_count{0u};

};

} /* namespace Detail { */

template <typename Actor, bool COUNT_PARTIAL = false>
using CountMaxActor = Detail::CountMaxActorImpl<Actor, COUNT_PARTIAL>;

} /* namespace sharemind { */

#endif /* SHAREMIND_COUNTMAXACTOR_H */
