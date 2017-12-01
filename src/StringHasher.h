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

#include <boost/functional/hash.hpp>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include "Concepts.h"
#include "Range.h"


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

    template <typename T>
    auto operator()(T && v) const noexcept
            -> SHAREMIND_REQUIRE_CONCEPTS_R(std::size_t,
                                            InputRangeTo(T, char),
                                            Not(DecaysTo(T, std::string)))
    { return this->operator()(std::begin(v), std::end(v)); }

    std::size_t operator()(std::string const & v) const noexcept
    { return boost::hash_range(v.begin(), v.end()); }

    std::size_t operator()(char const * const v) const noexcept
    { return this->operator()(v, Detail::TerminatingNullCharacterSentinel()); }

    template <typename Iterator_, typename Sentinel_>
    auto operator()(Iterator_ first, Sentinel_ last) const noexcept
            -> SHAREMIND_REQUIRE_CONCEPTS_R(std::size_t,
                                            InputIteratorTo(Iterator_, char),
                                            Sentinel(Sentinel_, Iterator_))
    {
        std::size_t seed = 0u;
        for (; first != last; ++first)
            boost::hash_combine(seed, *first);
        return seed;
    }

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_STRINGHASHER_H */
