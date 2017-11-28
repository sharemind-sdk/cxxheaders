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

#ifndef SHAREMIND_CONCEPTS_H
#define SHAREMIND_CONCEPTS_H

#include <cstddef>
#include <iterator>
#include <sharemind/compiler-support/GccVersion.h>
#include <type_traits>
#include <utility>
#include "TemplateAll.h"
#include "Void.h"


namespace sharemind {

struct Concept {};

template <typename ...> struct ValidTypes;

namespace Detail {

template <typename T>
using IteratorT = decltype(std::begin(std::declval<T &>()));
template <typename T>
using SentinelT = decltype(std::end(std::declval<T &>()));

enum class RequiresConceptResult { Succeed };

template <typename Concept, typename = void>
struct ModelsConcept: std::false_type {};

template <typename Concept, typename ... Ts>
struct ModelsConcept<
        Concept(Ts...),
        Void_t<decltype(std::declval<Concept>().check(std::declval<Ts>()...))>
    > : std::true_type
{};

} /* namespace Detail { */

template <typename ... Cs>
using Models = TemplateAll<Detail::ModelsConcept<Cs>::value...>;

#define SHAREMIND_REQUIRE(...) \
    typename ::std::enable_if< \
        __VA_ARGS__, \
        ::sharemind::Detail::RequiresConceptResult \
    >::type

#define SHAREMIND_REQUIRE_DEFAULTVALUE \
    ::sharemind::Detail::RequiresConceptResult::Succeed

#define SHAREMIND_REQUIRE_CONCEPTS(...) \
    SHAREMIND_REQUIRE(Models<__VA_ARGS__>::value)

#define SHAREMIND_REQUIRES(...) \
    SHAREMIND_REQUIRE(__VA_ARGS__) = SHAREMIND_REQUIRE_DEFAULTVALUE

#define SHAREMIND_REQUIRES_CONCEPTS(...) \
    SHAREMIND_REQUIRE_CONCEPTS(__VA_ARGS__) = SHAREMIND_REQUIRE_DEFAULTVALUE

#define SHAREMIND_DEFINE_CONCEPT(name) struct name: ::sharemind::Concept


SHAREMIND_DEFINE_CONCEPT(Same) {
    template <typename A, typename B>
    auto check(A &&, B &&) -> SHAREMIND_REQUIRE(std::is_same<A, B>::value);
};

SHAREMIND_DEFINE_CONCEPT(Not) {
    template <typename C, typename ... Ts>
    auto check(C(Ts...)) -> SHAREMIND_REQUIRE(!Models<C(Ts...)>::value);
};

SHAREMIND_DEFINE_CONCEPT(BaseOf) {
    template <typename A, typename B>
    auto check(A &&, B &&) -> SHAREMIND_REQUIRE(std::is_base_of<A, B>::value);
};

SHAREMIND_DEFINE_CONCEPT(DerivedFrom) {
    template <typename A, typename B>
    auto check(A &&, B &&) -> SHAREMIND_REQUIRE(std::is_base_of<B, A>::value);
};

SHAREMIND_DEFINE_CONCEPT(ConvertibleTo) {
    template <typename From, typename To>
    auto check(From &&, To &&)
            -> SHAREMIND_REQUIRE(std::is_convertible<From, To>::value);
};

SHAREMIND_DEFINE_CONCEPT(Callable) {
    template <typename T, typename ... Args>
    auto check(T && t, Args && ... args)
            -> decltype(t(std::forward<Args>(args)...));
};

SHAREMIND_DEFINE_CONCEPT(DefaultConstructible) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_default_constructible<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(NothrowDefaultConstructible) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(
                    std::is_nothrow_default_constructible<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(MoveConstructible) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_move_constructible<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(NothrowMoveConstructible) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_nothrow_move_constructible<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(MoveAssignable) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_move_assignable<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(NothrowMoveAssignable) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_nothrow_move_assignable<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(CopyConstructible) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_copy_constructible<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(NothrowCopyConstructible) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_nothrow_copy_constructible<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(CopyAssignable) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_copy_assignable<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(NothrowCopyAssignable) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_nothrow_copy_assignable<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(Destructible) {
    template <typename T>
    auto check(T && t)
            -> SHAREMIND_REQUIRE(std::is_destructible<T>::value);
};


namespace Detail {
namespace Concepts {

using ::std::swap;

#if !defined(SHAREMIND_GCC_VERSION) || SHAREMIND_GCC_VERSION >= 60000
template <typename T, typename U>
auto adl_swap(T && t, U && u) ->
        decltype(swap(std::forward<T>(t), std::forward<U>(u)));
#else
/* Work around GCC PR 63860: */
template <typename T, typename U>
auto adl_swap(T && t, U && u) -> ValidTypes<
            SHAREMIND_REQUIRE_CONCEPTS( \
                MoveConstructible(T), \
                MoveAssignable(T) \
            ), \
            decltype(swap(std::forward<T>(t), std::forward<U>(u)))
        >;
template <typename T, std::size_t N, typename U>
auto adl_swap(T (&t)[N], U (&u)[N]) -> ValidTypes<
            decltype(swap(std::forward<T>(t), std::forward<U>(u))),
            decltype(adl_swap(std::declval<T>(), std::declval<U>()))
        >;
#endif

} /* namespace Concepts { */
} /* namespace Detail { */

SHAREMIND_DEFINE_CONCEPT(Swappable) {
    template <typename T>
    auto check(T && t)
            -> decltype(Detail::Concepts::adl_swap(t, t));

    template <typename T, typename U>
    auto check(T && t, U && u) -> ValidTypes<
                decltype(Detail::Concepts::adl_swap(t, u)),
                decltype(Detail::Concepts::adl_swap(u, t))
            >;
};

#define SHAREMIND_CONCEPTS_H_(Name,op) \
    SHAREMIND_DEFINE_CONCEPT(Name ## Comparable) { \
        template <typename LHS, typename RHS> \
        auto check(LHS && l, RHS && r) -> SHAREMIND_REQUIRE_CONCEPTS( \
            ConvertibleTo(decltype(l op r), bool), \
            ConvertibleTo(decltype(l op std::declval<RHS const>()), bool), \
            ConvertibleTo(decltype(std::declval<LHS const>() op r), bool), \
            ConvertibleTo(decltype(std::declval<LHS const>() \
                                   op std::declval<RHS const>()), bool) \
        ); \
        template <typename T> \
        auto check(T && t) \
                -> decltype(check<T, T>(std::forward<T>(t), \
                                        std::forward<T>(t))); \
    };
SHAREMIND_CONCEPTS_H_(Equality,==)
SHAREMIND_CONCEPTS_H_(Inequality,!=)
SHAREMIND_CONCEPTS_H_(LessThan,<)
SHAREMIND_CONCEPTS_H_(LessOrEqual,<=)
SHAREMIND_CONCEPTS_H_(GreaterThan,>)
SHAREMIND_CONCEPTS_H_(GreaterOrEqual,>=)
#undef SHAREMIND_CONCEPTS_H_

SHAREMIND_DEFINE_CONCEPT(Integral) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE(std::is_integral<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(Signed) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE(std::is_signed<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(Unsigned) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE(std::is_unsigned<T>::value);
};

SHAREMIND_DEFINE_CONCEPT(SignedIntegral) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(Integral(T), Signed(T));
};

SHAREMIND_DEFINE_CONCEPT(UnsignedIntegral) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(Integral(T), Unsigned(T));
};

SHAREMIND_DEFINE_CONCEPT(Iterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(
                    CopyConstructible(T),
                    CopyAssignable(T),
                    Destructible(T),
                    Swappable(T &),
                    Same(decltype(*t),
                         typename std::iterator_traits<T>::reference),
                    Same(decltype(++t), T &)),
                typename std::iterator_traits<T>::value_type,
                typename std::iterator_traits<T>::difference_type,
                typename std::iterator_traits<T>::reference,
                typename std::iterator_traits<T>::pointer,
                typename std::iterator_traits<T>::iterator_category
            >;
};

SHAREMIND_DEFINE_CONCEPT(IteratorTo) {
    template <typename It, typename ValueType>
    auto check(It && it, ValueType && valueType) -> SHAREMIND_REQUIRE_CONCEPTS(
                Iterator(It),
                Same(typename std::iterator_traits<It>::value_type, ValueType)
            );
};

SHAREMIND_DEFINE_CONCEPT(InputIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(
                    Iterator(T),
                    EqualityComparable(T),
                    InequalityComparable(T),
                    ConvertibleTo(decltype(*t),
                                  typename std::iterator_traits<T>::value_type),
                    ConvertibleTo(decltype(*t++),
                                  typename std::iterator_traits<T>::value_type),
                    Same(decltype(*t),
                         typename std::iterator_traits<T>::reference)),
                decltype((void)t++)
            >;
};

SHAREMIND_DEFINE_CONCEPT(OutputIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
            SHAREMIND_REQUIRE_CONCEPTS(
                Iterator(T),
                ConvertibleTo(decltype(t++), T const &),
                Same(decltype(++t), T &)),
            decltype(*t =
                std::declval<typename std::iterator_traits<T>::value_type>()),
            decltype(*t++ =
                std::declval<typename std::iterator_traits<T>::value_type>())
        >;
};

SHAREMIND_DEFINE_CONCEPT(ForwardIterator) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                InputIterator(T),
                DefaultConstructible(T),
                ConvertibleTo(decltype(t++), T const &),
                Same(decltype(*t++),
                     typename std::iterator_traits<T>::reference),
                Same(typename std::iterator_traits<T>::reference,
                     typename std::conditional<
                         Models<OutputIterator(T)>::value,
                         typename std::iterator_traits<T>::value_type &,
                         typename std::iterator_traits<T>::value_type
                             const &
                     >::type)
            );
};

SHAREMIND_DEFINE_CONCEPT(BidirectionalIterator) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                ForwardIterator(T),
                ConvertibleTo(decltype(t--), T const &),
                Same(decltype(--t), T &),
                Same(decltype(*t--),
                     typename std::iterator_traits<T>::reference)
            );
};

SHAREMIND_DEFINE_CONCEPT(RandomAccessIterator) {

    template <typename T, typename DiffT>
    auto check_(T && t, DiffT n) -> SHAREMIND_REQUIRE_CONCEPTS(
                ConvertibleTo(decltype(t[n]),
                              typename std::iterator_traits<T>::reference),
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
                        std::declval<
                          typename std::iterator_traits<T>::difference_type>()))
    >;

};

SHAREMIND_DEFINE_CONCEPT(Range) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                ForwardIterator(Detail::IteratorT<T>),
                EqualityComparable(Detail::IteratorT<T>,
                                   Detail::SentinelT<T>),
                EqualityComparable(Detail::SentinelT<T>,
                                   Detail::IteratorT<T>),
                InequalityComparable(Detail::IteratorT<T>,
                                     Detail::SentinelT<T>),
                InequalityComparable(Detail::SentinelT<T>,
                                     Detail::IteratorT<T>)
            );
};

SHAREMIND_DEFINE_CONCEPT(RangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    Same(typename std::iterator_traits<
                                typename Detail::IteratorT<T> >::value_type,
                         ValueType)
                );
};

SHAREMIND_DEFINE_CONCEPT(BoundedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    Same(Detail::IteratorT<T>, Detail::SentinelT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(BoundedRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    RangeTo(T, ValueType),
                    Same(Detail::IteratorT<T>, Detail::SentinelT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(InputRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    InputIterator(Detail::IteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(InputRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    RangeTo(T, ValueType),
                    InputIterator(Detail::IteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(OutputRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    OutputIterator(Detail::IteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(OutputRangeTo) {
    template <typename T, typename ValueType>
    auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS(
                    RangeTo(T, ValueType),
                    OutputIterator(Detail::IteratorT<T>)
                );
};

SHAREMIND_DEFINE_CONCEPT(ValueSwappable) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                Iterator(T),
                Swappable(decltype(*t))
            );
};

SHAREMIND_DEFINE_CONCEPT(NullablePointer) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                EqualityComparable(T),
                InequalityComparable(T),
                DefaultConstructible(T),
                CopyConstructible(T),
                CopyAssignable(T),
                Destructible(T),
                Swappable(T &),
                ConvertibleTo(T, bool),
                ConvertibleTo(decltype(t == nullptr), bool),
                ConvertibleTo(decltype(nullptr == t), bool),
                ConvertibleTo(decltype(t != nullptr), bool),
                ConvertibleTo(decltype(nullptr != t), bool),
                Same(decltype(t = nullptr), T &)
            );
};

SHAREMIND_DEFINE_CONCEPT(Hash) {
    template <typename T, typename Key>
    auto check(T && t, Key && key) -> ValidTypes<
                SHAREMIND_REQUIRE(std::is_object<T>::value),
                SHAREMIND_REQUIRE_CONCEPTS(
                    CopyConstructible(T),
                    Destructible(T),
                    Same(decltype(std::declval<T &>()(std::declval<Key>())),
                         std::size_t),
                    Same(decltype(std::declval<T&>()(
                                      std::declval<Key const>())),
                         std::size_t),
                    Same(decltype(std::declval<T const &>()(
                                      std::declval<Key>())),
                         std::size_t),
                    Same(decltype(std::declval<T const &>()(
                                      std::declval<Key const>())),
                         std::size_t)
                )
            >;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_CONCEPTS_H */
