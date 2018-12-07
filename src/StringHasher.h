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

#ifndef SHAREMIND_STRINGHASHER_H
#define SHAREMIND_STRINGHASHER_H

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>
#include <utility>
#include "Concepts.h"
#include "Range.h"
#include "RemoveCvref.h"


namespace sharemind {
namespace Detail {

struct TerminatingNullCharacterSentinel {};

template <typename Iter>
auto operator==(TerminatingNullCharacterSentinel const &, Iter const & it)
        noexcept(noexcept((*std::declval<Iter const &>()) == '\0'))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InputIterator(Iter))
{ return (*it) == '\0'; }

template <typename Iter>
auto operator==(Iter const & it, TerminatingNullCharacterSentinel const &)
        noexcept(noexcept((*std::declval<Iter const &>()) == '\0'))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InputIterator(Iter))
{ return (*it) == '\0'; }

template <typename Iter>
auto operator!=(TerminatingNullCharacterSentinel const &, Iter const & it)
        noexcept(noexcept((*std::declval<Iter const &>()) == '\0'))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InputIterator(Iter))
{ return (*it) != '\0'; }

template <typename Iter>
auto operator!=(Iter const & it, TerminatingNullCharacterSentinel const &)
        noexcept(noexcept((*std::declval<Iter const &>()) == '\0'))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InputIterator(Iter))
{ return (*it) != '\0'; }

} /* namespace Detail { */

struct StringHasher {

    struct transparent_key_equal {

        using is_transparent = void;

        template <typename T>
        auto operator()(std::string const & a, T && b) const noexcept
            -> SHAREMIND_REQUIRE_CONCEPTS_R(bool,
                                            RandomAccessRangeTo(T, char),
                                            BoundedRange(T))
        {
            auto bIt(std::begin(b));
            auto bEnd(std::end(b)); // Don't change to const, might break stuff
            if (!integralEqual(a.size(), std::distance(bIt, bEnd)))
                return false;
            return std::equal(bIt, bEnd, a.begin());
        }

        template <typename T>
        auto operator()(std::string const & a, T && b) const noexcept
                -> SHAREMIND_REQUIRE_CONCEPTS_R(
                        bool,
                        InputRangeTo(T, char),
                        Not(All(RandomAccessRange(T), BoundedRange(T)))
                )
        {
            auto aIt(a.begin());
            auto aEnd(a.end()); // Don't change to const, might break stuff
            auto bIt(std::begin(b));
            auto bEnd(std::end(b)); // Don't change to const, might break stuff
            for (; aIt != aEnd; ++aIt, ++bIt)
                if ((bIt == bEnd) || (*aIt != *bIt))
                    return false;
            return bIt == bEnd;
        }

        template <typename T>
        auto operator()(T && a, std::string const & b) const noexcept
                -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InputRangeTo(T, char))
        { return this->operator()(b, std::forward<T>(a)); }

        bool operator()(std::string const & a, std::string const & b)
                const noexcept
        { return a == b; }

        bool operator()(char const * const a, std::string const & b)
                const noexcept
        { return std::strcmp(a, b.c_str()) == 0; }

        bool operator()(std::string const & a, char const * const b)
                const noexcept
        { return std::strcmp(a.c_str(), b) == 0; }

    };

    template <typename Iterator_, typename Sentinel_>
    auto operator()(Iterator_ first, Sentinel_ last) const
            noexcept(noexcept(first != last)
                     && noexcept(++first)
                     && noexcept(*first))
            -> SHAREMIND_REQUIRE_CONCEPTS_R(std::size_t,
                                            InputIteratorTo(Iterator_, char),
                                            Sentinel(Sentinel_, Iterator_))
    {
        std::size_t seed = 0u;
        for (; first != last; ++first)
            boost::hash_combine(seed, *first);
        return seed;
    }

    std::size_t operator()(std::string const & v) const
            noexcept(noexcept(std::declval<StringHasher const &>()(v.begin(),
                                                                   v.end())))
    { return boost::hash_range(v.begin(), v.end()); }

    std::size_t operator()(char const * const v) const noexcept
    { return this->operator()(v, Detail::TerminatingNullCharacterSentinel()); }

    template <typename T>
    auto operator()(T && v) const
            noexcept(noexcept(std::declval<StringHasher const &>()(
                                  std::begin(v),
                                  std::end(v))))
            -> SHAREMIND_REQUIRE_CONCEPTS_R(std::size_t,
                                            InputRangeTo(T, char),
                                            Not(DecaysTo(T, std::string)))
    { return this->operator()(std::begin(v), std::end(v)); }

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_STRINGHASHER_H */
