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

#ifndef SHAREMIND_REVERSEDRANGE_H
#define SHAREMIND_REVERSEDRANGE_H

#include <type_traits>
#include "Concepts.h"
#include "Iterator.h"
#include "Range.h"
#include "RemoveCvref.h"


namespace sharemind {

template <typename T>
using ReversedRangeIteratorT = decltype(std::rbegin(std::declval<T &>()));

template <typename T>
using ReversedRangeSentinelT = decltype(std::rend(std::declval<T &>()));

#define SHAREMIND_REVERSED_RANGE_H_TO(Name) \
    SHAREMIND_DEFINE_CONCEPT(Name ## To) { \
        template <typename T, typename ValueType> \
        auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS( \
                        Name(T), \
                        Same(IteratorValueTypeT<ReversedRangeIteratorT<T> >, \
                             ValueType) \
                    ); \
    }


SHAREMIND_DEFINE_CONCEPT(ReversedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                Iterator(ReversedRangeIteratorT<T>),
                Sentinel(ReversedRangeSentinelT<T>, ReversedRangeIteratorT<T>)
            );
};
SHAREMIND_REVERSED_RANGE_H_TO(ReversedRange);

SHAREMIND_DEFINE_CONCEPT(BoundedReversedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    Same(ReversedRangeIteratorT<T>, ReversedRangeSentinelT<T>)
                );
};
SHAREMIND_REVERSED_RANGE_H_TO(BoundedReversedRange);

SHAREMIND_DEFINE_CONCEPT(SizedReversedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
        Range(T),
        ConvertibleTo(
            decltype(
                size(std::declval<
                        typename std::remove_reference<T>::type const &>())),
            typename std::iterator_traits<
                    ReversedRangeIteratorT<T> >::difference_type)
    );
};
SHAREMIND_REVERSED_RANGE_H_TO(SizedReversedRange);

#define SHAREMIND_REVERSED_RANGE_H_CHAIN(Prefix, Base) \
    SHAREMIND_DEFINE_CONCEPT(Prefix ## ReversedRange) { \
        template <typename T> \
        auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS( \
                        Base(T), \
                        Prefix ## Iterator(ReversedRangeIteratorT<T>) \
                    ); \
    }; \
    SHAREMIND_REVERSED_RANGE_H_TO(Prefix ## ReversedRange)

SHAREMIND_REVERSED_RANGE_H_CHAIN(Input, ReversedRange);
SHAREMIND_REVERSED_RANGE_H_CHAIN(Output, ReversedRange);
SHAREMIND_REVERSED_RANGE_H_CHAIN(Forward, InputReversedRange);
SHAREMIND_REVERSED_RANGE_H_CHAIN(Bidirectional, ForwardReversedRange);
SHAREMIND_REVERSED_RANGE_H_CHAIN(RandomAccess, BidirectionalReversedRange);

SHAREMIND_DEFINE_CONCEPT(ConstantTimeMeasurableReversedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
        Any(All(BoundedReversedRange(T),
                RandomAccessReversedRange(T)),
            SizedReversedRange(T)));
};
SHAREMIND_REVERSED_RANGE_H_TO(ConstantTimeMeasurableReversedRange);

#undef SHAREMIND_REVERSED_RANGE_H_CHAIN
#undef SHAREMIND_REVERSED_RANGE_H_TO

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(SizedRange(T)),
                SizedReversedRange(T))>
auto measureRange(T && t) noexcept(noexcept(size(std::declval<T &&>())))
{ return size(std::forward<T>(t)); }

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(SizedRange(T)),
                Not(SizedReversedRange(T)),
                BoundedReversedRange(T),
                RandomAccessReversedRange(T))>
auto measureRange(T && t)
    noexcept(noexcept(std::rend(std::declval<T &>())
                      - std::rbegin(std::declval<T &>())))
{ return std::rend(t) - std::rbegin(t); }

namespace Detail {

#define SHAREMIND_REVERSEDRANGE_H_ \
    auto begin() { return std::rbegin(m_range); } \
    auto end() { return std::rend(m_range); } \
    auto begin() const \
    { return std::rbegin(static_cast<RemoveCvrefT<T> const &>(m_range)); } \
    auto end() const \
    { return std::rend(static_cast<RemoveCvrefT<T> const &>(m_range)); } \
    T & m_range;
template <typename T, bool = Models<SizedReversedRange(T)>::value>
struct RangeReversionWrapperImpl { SHAREMIND_REVERSEDRANGE_H_ };
template <typename T>
struct RangeReversionWrapperImpl<T, true> {
    SHAREMIND_REVERSEDRANGE_H_
    auto size() const { return this->m_range.size(); }
};
#undef SHAREMIND_REVERSEDRANGE_H_

} // namespace Detail

template <typename T>
using RangeReversionWrapper = Detail::RangeReversionWrapperImpl<T>;

template <typename T>
auto reverseRange(T && t) { return RangeReversionWrapper<T>{t}; }

} /* namespace Sharemind { */

#endif /* SHAREMIND_REVERSEDRANGE_H */
