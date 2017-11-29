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

#include <iterator>
#include <utility>
#include <type_traits>
#include "Concepts.h"
#include "Size.h"


namespace sharemind {

template <typename T>
using RangeIteratorT = decltype(std::begin(std::declval<T &>()));

template <typename T>
using RangeSentinelT = decltype(std::end(std::declval<T &>()));


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

SHAREMIND_DEFINE_CONCEPT(RangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    Same(typename std::iterator_traits<
                                RangeIteratorT<T> >::value_type,
                         ValueType)
                );
};

SHAREMIND_DEFINE_CONCEPT(BoundedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    Same(RangeIteratorT<T>, RangeSentinelT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(BoundedRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    RangeTo(T, ValueType),
                    Same(RangeIteratorT<T>, RangeSentinelT<T>)
                );
};

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

SHAREMIND_DEFINE_CONCEPT(SizedRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
        RangeTo(T, ValueType),
        ConvertibleTo(
            decltype(
                size(std::declval<
                        typename std::remove_reference<T>::type const &>())),
            typename std::iterator_traits<RangeIteratorT<T> >::difference_type)
    );
};

SHAREMIND_DEFINE_CONCEPT(InputRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    InputIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(InputRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    RangeTo(T, ValueType),
                    InputIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(OutputRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    OutputIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(OutputRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    RangeTo(T, ValueType),
                    OutputIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(ForwardRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    InputRange(T),
                    ForwardIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(ForwardRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    InputRangeTo(T, ValueType),
                    ForwardIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(BidirectionalRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    ForwardRange(T),
                    BidirectionalIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(BidirectionalRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    ForwardRangeTo(T, ValueType),
                    BidirectionalIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(RandomAccessRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    BidirectionalRange(T),
                    RandomAccessIterator(RangeIteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(RandomAccessRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    BidirectionalRangeTo(T, ValueType),
                    RandomAccessIterator(RangeIteratorT<T>)
                );
};

} /* namespace Sharemind { */


#endif /* SHAREMIND_RANGE_H */
