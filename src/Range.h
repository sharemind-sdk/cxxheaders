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

#ifndef SHAREMIND_RANGE_H
#define SHAREMIND_RANGE_H

#include <cstddef>
#include <iterator>
#include <utility>
#include <type_traits>
#include "Concepts.h"
#include "Iterator.h"
#include "Size.h"


namespace sharemind {

template <typename T>
using RangeIteratorT = decltype(std::begin(std::declval<T &>()));

template <typename T>
using RangeSentinelT = decltype(std::end(std::declval<T &>()));


#define SHAREMIND_RANGE_H_TO(Name) \
    SHAREMIND_DEFINE_CONCEPT(Name ## To) { \
        template <typename T, typename ValueType> \
        auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS( \
                        Name(T), \
                        Same(IteratorValueTypeT<RangeIteratorT<T> >, \
                             ValueType) \
                    ); \
    }


SHAREMIND_DEFINE_CONCEPT(Range) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                ForwardIterator(RangeIteratorT<T>),
                EqualityComparable(RangeIteratorT<T>, RangeSentinelT<T>),
                EqualityComparable(RangeSentinelT<T>, RangeIteratorT<T>),
                InequalityComparable(RangeIteratorT<T>, RangeSentinelT<T>),
                InequalityComparable(RangeSentinelT<T>, RangeIteratorT<T>)
            );
};
SHAREMIND_RANGE_H_TO(Range);

SHAREMIND_DEFINE_CONCEPT(BoundedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    Same(RangeIteratorT<T>, RangeSentinelT<T>)
                );
};
SHAREMIND_RANGE_H_TO(BoundedRange);

SHAREMIND_DEFINE_CONCEPT(SizedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
        Range(T),
        ConvertibleTo(
            decltype(
                size(std::declval<
                        typename std::remove_reference<T>::type const &>())),
            typename std::iterator_traits<RangeIteratorT<T> >::difference_type)
    );
};
SHAREMIND_RANGE_H_TO(SizedRange);

#define SHAREMIND_RANGE_H_CHAIN(Prefix, Base) \
    SHAREMIND_DEFINE_CONCEPT(Prefix ## Range) { \
        template <typename T> \
        auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS( \
                        Base(T), \
                        Prefix ## Iterator(RangeIteratorT<T>) \
                    ); \
    }; \
    SHAREMIND_RANGE_H_TO(Prefix ## Range)

SHAREMIND_RANGE_H_CHAIN(Input, Range);
SHAREMIND_RANGE_H_CHAIN(Output, Range);
SHAREMIND_RANGE_H_CHAIN(Forward, InputRange);
SHAREMIND_RANGE_H_CHAIN(Bidirectional, ForwardRange);
SHAREMIND_RANGE_H_CHAIN(RandomAccess, BidirectionalRange);

SHAREMIND_DEFINE_CONCEPT(MeasurableRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
        Any(All(BoundedRange(T), RandomAccessRange(T)), SizedRange(T)));
};
SHAREMIND_RANGE_H_TO(MeasurableRange);

#undef SHAREMIND_RANGE_H_CHAIN
#undef SHAREMIND_RANGE_H_TO

template <typename CharT, std::size_t N>
class LiteralStringRange {

    static_assert(N > 0u, "");
    static_assert(std::is_const<CharT>::value, "");

public: /* Methods: */

    LiteralStringRange(CharT (& begin)[N]) : m_begin(begin) {}

    CharT * begin()  const noexcept { return m_begin; }
    CharT * cbegin() const noexcept { return m_begin; }
    CharT * end()  const noexcept   { return m_begin + size(); }
    CharT * cend() const noexcept   { return m_begin + size(); }
    constexpr static std::size_t size() noexcept { return N - 1u; }

private: /* Types: */

    CharT * m_begin;

};

template <typename CharT, std::size_t N>
constexpr LiteralStringRange<CharT, N> asLiteralStringRange(
        CharT (& literal)[N]) noexcept
{ return LiteralStringRange<CharT const, N>(literal); }

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(SizedRange(T))>
auto measureRange(T && t) noexcept(noexcept(size(std::declval<T &&>())))
        -> decltype(size(std::forward<T>(t)))
{ return size(std::forward<T>(t)); }

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(SizedRange(T)),
                BoundedRange(T),
                RandomAccessRange(T))>
auto measureRange(T && t)
    noexcept(noexcept(std::end(std::declval<T &>())
                      - std::begin(std::declval<T &>())))
        -> decltype(std::end(t) - std::begin(t))
{ return std::end(t) - std::begin(t); }

} /* namespace Sharemind { */


#endif /* SHAREMIND_RANGE_H */
