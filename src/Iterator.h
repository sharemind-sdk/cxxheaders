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

#ifndef SHAREMIND_ITERATOR_H
#define SHAREMIND_ITERATOR_H

#include <iterator>
#include <utility>
#include <type_traits>
#include "Concepts.h"


namespace sharemind {

template <typename T>
using IteratorCategoryT = typename std::iterator_traits<T>::iterator_category;

template <typename T>
using IteratorValueTypeT = typename std::iterator_traits<T>::value_type;

template <typename T>
using IteratorDifferenceTypeT =
        typename std::iterator_traits<T>::difference_type;

template <typename T>
using IteratorPointerT = typename std::iterator_traits<T>::pointer;

template <typename T>
using IteratorReferenceT = typename std::iterator_traits<T>::reference;


#define SHAREMIND_ITERATOR_H_TO(Name) \
    SHAREMIND_DEFINE_CONCEPT(Name ## To) { \
        template <typename It, typename ValueType> \
        auto check(It && it, ValueType && valueType)  \
                -> SHAREMIND_REQUIRE_CONCEPTS( \
                    Name(It), \
                    Same(IteratorValueTypeT<It>, ValueType) \
                ); \
    };

SHAREMIND_DEFINE_CONCEPT(Iterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(
                    CopyConstructible(T),
                    CopyAssignable(T),
                    Destructible(T),
                    Swappable(T &),
                    Same(decltype(*t), IteratorReferenceT<T>),
                    Same(decltype(++t), T &)),
                IteratorCategoryT<T>,
                IteratorValueTypeT<T>,
                IteratorDifferenceTypeT<T>,
                IteratorPointerT<T>,
                IteratorReferenceT<T>
            >;
};
SHAREMIND_ITERATOR_H_TO(Iterator)


SHAREMIND_DEFINE_CONCEPT(InputIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(
                    Iterator(T),
                    EqualityComparable(T),
                    InequalityComparable(T),
                    ConvertibleTo(decltype(*t), IteratorValueTypeT<T>),
                    ConvertibleTo(decltype(*t++), IteratorValueTypeT<T>),
                    Same(decltype(*t), IteratorReferenceT<T>)),
                decltype((void)t++)
            >;
};
SHAREMIND_ITERATOR_H_TO(InputIterator)


SHAREMIND_DEFINE_CONCEPT(OutputIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
            SHAREMIND_REQUIRE_CONCEPTS(
                Iterator(T),
                ConvertibleTo(decltype(t++), T const &),
                Same(decltype(++t), T &)),
            decltype(*t =
                std::declval<IteratorValueTypeT<T> >()),
            decltype(*t++ =
                std::declval<IteratorValueTypeT<T> >())
        >;
};
SHAREMIND_ITERATOR_H_TO(OutputIterator)


SHAREMIND_DEFINE_CONCEPT(ForwardIterator) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                InputIterator(T),
                DefaultConstructible(T),
                ConvertibleTo(decltype(t++), T const &),
                Same(decltype(*t++), IteratorReferenceT<T>),
                Same(IteratorReferenceT<T>,
                     typename std::conditional<
                         Models<OutputIterator(T)>::value,
                         IteratorValueTypeT<T> &,
                         IteratorValueTypeT<T> const &
                     >::type)
            );
};
SHAREMIND_ITERATOR_H_TO(ForwardIterator)


SHAREMIND_DEFINE_CONCEPT(BidirectionalIterator) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                ForwardIterator(T),
                ConvertibleTo(decltype(t--), T const &),
                Same(decltype(--t), T &),
                Same(decltype(*t--), IteratorReferenceT<T>)
            );
};
SHAREMIND_ITERATOR_H_TO(BidirectionalIterator)


SHAREMIND_DEFINE_CONCEPT(RandomAccessIterator) {

    template <typename T, typename DiffT>
    auto check_(T && t, DiffT n) -> SHAREMIND_REQUIRE_CONCEPTS(
                ConvertibleTo(decltype(t[n]), IteratorReferenceT<T>),
                Same(decltype(t += n), T &),
                Same(decltype(t -= n), T &),
                Same(decltype(t + n), T),
                Same(decltype(n + t), T),
                Same(decltype(t - n), T),
                Same(decltype(t - t), DiffT)
            );

    template <typename T>
    auto check(T && t) -> ValidTypes<
        SHAREMIND_REQUIRE_CONCEPTS(
                BidirectionalIterator(T),
                LessThanComparable(T),
                LessOrEqualComparable(T),
                GreaterThanComparable(T),
                GreaterOrEqualComparable(T)),
        decltype(check_(std::forward<T>(t),
                        std::declval<IteratorDifferenceTypeT<T> >()))
    >;

};
SHAREMIND_ITERATOR_H_TO(RandomAccessIterator)


#undef SHAREMIND_ITERATOR_H_TO


SHAREMIND_DEFINE_CONCEPT(ValueSwappable) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                Iterator(T),
                Swappable(decltype(*t))
            );
};

} /* namespace sharemind { */

#endif /* SHAREMIND_ITERATOR_H */
