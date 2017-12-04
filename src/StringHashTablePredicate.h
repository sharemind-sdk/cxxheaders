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

#ifndef SHAREMIND_STRINGHASHTABLEPREDICATE_H
#define SHAREMIND_STRINGHASHTABLEPREDICATE_H

#include <cstddef>
#include <string>
#include <type_traits>
#include "Concepts.h"
#include "HashTablePredicate.h"
#include "Range.h"
#include "StringHasher.h"


namespace sharemind {

using StringHashTablePredicate = HashTablePredicate<std::string>;

namespace Detail {

class StringRefStringHashTablePredicate {

public: /* Methods: */

    StringRefStringHashTablePredicate(std::string const & s)
            noexcept(noexcept(StringHasher()(s)))
        : m_hash(StringHasher()(s))
        , m_string(&s)
    {}

    StringRefStringHashTablePredicate(
            StringRefStringHashTablePredicate &&) noexcept = default;
    StringRefStringHashTablePredicate(
            StringRefStringHashTablePredicate const &) noexcept = default;
    StringRefStringHashTablePredicate & operator=(
            StringRefStringHashTablePredicate &&) noexcept = default;
    StringRefStringHashTablePredicate & operator=(
            StringRefStringHashTablePredicate const &) noexcept = default;

    std::size_t hash() const noexcept { return m_hash; }
    bool operator()(std::string const & s) const { return s == *m_string; }

private: /* Fields: */

    std::size_t m_hash;
    std::string const * m_string;

};
static_assert(
    Models<StringHashTablePredicate(StringRefStringHashTablePredicate)>::value,
    "");

class CStringRefStringHashTablePredicate {

public: /* Methods: */

    CStringRefStringHashTablePredicate(char const * const s)
            noexcept(noexcept(StringHasher()(s)))
        : m_hash(StringHasher()(s))
        , m_string(s)
    {}

    CStringRefStringHashTablePredicate(
            CStringRefStringHashTablePredicate &&) noexcept = default;
    CStringRefStringHashTablePredicate(
            CStringRefStringHashTablePredicate const &) noexcept = default;
    CStringRefStringHashTablePredicate & operator=(
            CStringRefStringHashTablePredicate &&) noexcept = default;
    CStringRefStringHashTablePredicate & operator=(
            CStringRefStringHashTablePredicate const &) noexcept = default;

    std::size_t hash() const noexcept { return m_hash; }
    bool operator()(std::string const & s) const { return s == m_string; }

private: /* Fields: */

    std::size_t m_hash;
    char const * m_string;

};
static_assert(
    Models<StringHashTablePredicate(CStringRefStringHashTablePredicate)>::value,
    "");

template <typename Range>
class RangeRefStringHashTablePredicate {

public: /* Methods: */

    RangeRefStringHashTablePredicate(Range & range)
            noexcept(noexcept(StringHasher()(range)))
        : m_hash(StringHasher()(range))
        , m_range(range)
    {}

    RangeRefStringHashTablePredicate(
            RangeRefStringHashTablePredicate &&) noexcept = default;
    RangeRefStringHashTablePredicate(
            RangeRefStringHashTablePredicate const &) noexcept = default;
    RangeRefStringHashTablePredicate & operator=(
            RangeRefStringHashTablePredicate &&) noexcept = delete;
    RangeRefStringHashTablePredicate & operator=(
            RangeRefStringHashTablePredicate const &) noexcept = delete;

    std::size_t hash() const noexcept { return m_hash; }

    bool operator()(std::string const & s) const
    { return rangeEqual(s, m_range); } // Might throw

private: /* Fields: */

    std::size_t m_hash;
    Range & m_range;

};

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(DecaysTo(T, std::string))>
StringRefStringHashTablePredicate createTemporaryStringHashTablePredicate(
        T && str) noexcept(noexcept(StringRefStringHashTablePredicate(str)))
{ return StringRefStringHashTablePredicate(str); }

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                ConvertibleTo(typename std::decay<T>::type, char const *),
                Not(InputRangeTo(T, char)))>
CStringRefStringHashTablePredicate createTemporaryStringHashTablePredicate(
        T && str) noexcept(noexcept(CStringRefStringHashTablePredicate(str)))
{ return CStringRefStringHashTablePredicate(str); }

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(DecaysTo(T, std::string)),
                InputRangeTo(T, char))>
RangeRefStringHashTablePredicate<T> createTemporaryStringHashTablePredicate(
        T && range)
        noexcept(noexcept(RangeRefStringHashTablePredicate<T>(range)))
{
    static_assert(Models<sharemind::StringHashTablePredicate(
                      RangeRefStringHashTablePredicate<T>)>::value, "");
    return RangeRefStringHashTablePredicate<T>(range);
}

} /* namespace Detail { */

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(sharemind::StringHashTablePredicate(T))>
T && getOrCreateTemporaryStringHashTablePredicate(T && pred) noexcept
{ return std::forward<T>(pred); }

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(sharemind::StringHashTablePredicate(T)))>
auto getOrCreateTemporaryStringHashTablePredicate(T && pred)
        noexcept(noexcept(Detail::createTemporaryStringHashTablePredicate(
                              std::forward<T>(pred))))
        -> decltype(Detail::createTemporaryStringHashTablePredicate(
                        std::forward<T>(pred)))
{
    return Detail::createTemporaryStringHashTablePredicate(
                std::forward<T>(pred));
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_STRINGHASHTABLEPREDICATE_H */
