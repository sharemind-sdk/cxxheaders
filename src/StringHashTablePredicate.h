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
        : StringRefStringHashTablePredicate(s, StringHasher()(s))
    {}

    constexpr StringRefStringHashTablePredicate(std::string const & s,
                                                std::size_t hash) noexcept
        : m_string(&s)
        , m_hash(hash)
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

    std::string const * m_string;
    std::size_t m_hash;

};
static_assert(
    Models<StringHashTablePredicate(StringRefStringHashTablePredicate)>::value,
    "");

class CStringRefStringHashTablePredicate {

public: /* Methods: */

    CStringRefStringHashTablePredicate(char const * const s)
            noexcept(noexcept(StringHasher()(s)))
        : CStringRefStringHashTablePredicate(s, StringHasher()(s))
    {}

    constexpr CStringRefStringHashTablePredicate(char const * const s,
                                                 std::size_t hash) noexcept
        : m_string(s)
        , m_hash(hash)
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

    char const * m_string;
    std::size_t m_hash;

};
static_assert(
    Models<StringHashTablePredicate(CStringRefStringHashTablePredicate)>::value,
    "");

template <typename Range>
class RangeRefStringHashTablePredicate {

    static_assert(!std::is_reference<Range>::value, "");

public: /* Methods: */

    RangeRefStringHashTablePredicate(Range & range)
            noexcept(noexcept(StringHasher()(range)))
        : RangeRefStringHashTablePredicate(range, StringHasher()(range))
    {}

    constexpr RangeRefStringHashTablePredicate(Range & range,
                                               std::size_t hash) noexcept
        : m_range(&range)
        , m_hash(hash)
    {}

    RangeRefStringHashTablePredicate(
            RangeRefStringHashTablePredicate &&) noexcept = default;
    RangeRefStringHashTablePredicate(
            RangeRefStringHashTablePredicate const &) noexcept = default;
    RangeRefStringHashTablePredicate & operator=(
            RangeRefStringHashTablePredicate &&) noexcept = default;
    RangeRefStringHashTablePredicate & operator=(
            RangeRefStringHashTablePredicate const &) noexcept = default;

    std::size_t hash() const noexcept { return m_hash; }

    bool operator()(std::string const & s) const
    { return rangeEqual(s, *m_range); } // Might throw

private: /* Fields: */

    Range * m_range;
    std::size_t m_hash;

};

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(DecaysTo(T, std::string))>
StringRefStringHashTablePredicate createTemporaryStringHashTablePredicate(
        T && str) noexcept(noexcept(StringRefStringHashTablePredicate(str)))
{ return StringRefStringHashTablePredicate(str); }

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(DecaysTo(T, std::string))>
constexpr
StringRefStringHashTablePredicate createTemporaryStringHashTablePredicate(
        T && str,
        std::size_t hash) noexcept
{
    static_assert(noexcept(StringRefStringHashTablePredicate(str, hash)), "");
    return StringRefStringHashTablePredicate(str, hash);
}

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                ConvertibleTo(typename std::decay<T>::type, char const *),
                Not(InputRangeTo(T, char)))>
CStringRefStringHashTablePredicate createTemporaryStringHashTablePredicate(
        T && str) noexcept(noexcept(CStringRefStringHashTablePredicate(str)))
{ return CStringRefStringHashTablePredicate(str); }

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                ConvertibleTo(typename std::decay<T>::type, char const *),
                Not(InputRangeTo(T, char)))>
constexpr
CStringRefStringHashTablePredicate createTemporaryStringHashTablePredicate(
        T && str,
        std::size_t hash) noexcept
{
    static_assert(noexcept(CStringRefStringHashTablePredicate(str, hash)), "");
    return CStringRefStringHashTablePredicate(str, hash);
}

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(DecaysTo(T, std::string)),
                InputRangeTo(T, char))>
RangeRefStringHashTablePredicate<typename std::remove_reference<T>::type>
createTemporaryStringHashTablePredicate(T && range)
        noexcept(noexcept(RangeRefStringHashTablePredicate<
                                typename std::remove_reference<T>::type
                          >(range)))
{
    using R =
            RangeRefStringHashTablePredicate<
                typename std::remove_reference<T>::type>;
    static_assert(Models<sharemind::StringHashTablePredicate(R)>::value, "");
    return R(range);
}

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(DecaysTo(T, std::string)),
                InputRangeTo(T, char))>
RangeRefStringHashTablePredicate<typename std::remove_reference<T>::type>
constexpr createTemporaryStringHashTablePredicate(T && range,
                                                  std::size_t hash) noexcept
{
    using R =
            RangeRefStringHashTablePredicate<
                typename std::remove_reference<T>::type>;
    static_assert(Models<sharemind::StringHashTablePredicate(R)>::value, "");
    static_assert(noexcept(R(range, hash)), "");
    return R(range, hash);
}

} /* namespace Detail { */

/**
  \warning The reference passed to this function must stay valid as long as the
           return result is alive, because no copies are made and ownership is
           not taken. The result contains a plain pointer or a reference to the
           argument which will otherwise become dangling.
*/
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(sharemind::StringHashTablePredicate(T))>
constexpr T && getOrCreateTemporaryStringHashTablePredicate(T && pred) noexcept
{ return std::forward<T>(pred); }

/**
  \warning The reference passed to this function must stay valid as long as the
           return result is alive, because no copies are made and ownership is
           not taken. The result contains a plain pointer or a reference to the
           argument which will otherwise become dangling.
*/
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(sharemind::StringHashTablePredicate(T)))>
auto getOrCreateTemporaryStringHashTablePredicate(T && t)
        noexcept(noexcept(Detail::createTemporaryStringHashTablePredicate(
                              std::forward<T>(t))))
        -> decltype(Detail::createTemporaryStringHashTablePredicate(
                        std::forward<T>(t)))
{ return Detail::createTemporaryStringHashTablePredicate(std::forward<T>(t)); }

/**
  \warning The reference passed to this function must stay valid as long as the
           return result is alive, because no copies are made and ownership is
           not taken. The result contains a plain pointer or a reference to the
           argument which will otherwise become dangling.
*/
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(sharemind::StringHashTablePredicate(T)))>
constexpr auto getOrCreateTemporaryStringHashTablePredicate(
        T && t,
        std::size_t hash) noexcept
        -> decltype(Detail::createTemporaryStringHashTablePredicate(
                        std::forward<T>(t), hash))
{
    static_assert(noexcept(
                      Detail::createTemporaryStringHashTablePredicate(
                          std::forward<T>(t), hash)), "");
    return Detail::createTemporaryStringHashTablePredicate(std::forward<T>(t),
                                                           hash);
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_STRINGHASHTABLEPREDICATE_H */
