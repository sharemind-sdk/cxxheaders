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
            decltype(swap(std::forward<T>(t), std::forward<U>(u))),
            SHAREMIND_REQUIRE(std::is_move_constructible<T>::value),
            SHAREMIND_REQUIRE(std::is_move_assignable<T>::value)
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
        template <typename T> \
        auto check(T && t) \
                -> SHAREMIND_REQUIRE( \
                        std::is_convertible<decltype(t op t), bool>::value \
                        && std::is_convertible< \
                            decltype(t op std::declval<T const>()),  \
                            bool>::value \
                        && std::is_convertible< \
                            decltype(std::declval<T const>() op t),  \
                            bool>::value \
                        && std::is_convertible< \
                            decltype(std::declval<T const>() \
                                     op std::declval<T const>()), \
                            bool \
                        >::value \
                    ); \
    };
SHAREMIND_CONCEPTS_H_(Equality,==)
SHAREMIND_CONCEPTS_H_(Inequality,!=)
SHAREMIND_CONCEPTS_H_(LessThan,<)
SHAREMIND_CONCEPTS_H_(LessOrEqual,<=)
SHAREMIND_CONCEPTS_H_(GreaterThan,>)
SHAREMIND_CONCEPTS_H_(GreaterOrEqual,>=)
#undef SHAREMIND_CONCEPTS_H_

SHAREMIND_DEFINE_CONCEPT(Iterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(CopyConstructible(T)),
                SHAREMIND_REQUIRE_CONCEPTS(CopyAssignable(T)),
                SHAREMIND_REQUIRE_CONCEPTS(Destructible(T)),
                SHAREMIND_REQUIRE_CONCEPTS(Swappable(T &)),
                typename std::iterator_traits<T>::value_type,
                typename std::iterator_traits<T>::difference_type,
                typename std::iterator_traits<T>::reference,
                typename std::iterator_traits<T>::pointer,
                typename std::iterator_traits<T>::iterator_category,
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(*t),
                            typename std::iterator_traits<T>::reference
                        >::value),
                SHAREMIND_REQUIRE(std::is_same<decltype(++t), T &>::value)
            >;
};

SHAREMIND_DEFINE_CONCEPT(InputIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(Iterator(T)),
                SHAREMIND_REQUIRE_CONCEPTS(EqualityComparable(T)),
                SHAREMIND_REQUIRE_CONCEPTS(InequalityComparable(T)),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(*t),
                            typename std::iterator_traits<T>::reference
                        >::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<
                            decltype(*t),
                            typename std::iterator_traits<T>::value_type
                        >::value),
                decltype((void)t++),
                SHAREMIND_REQUIRE(
                        std::is_convertible<
                            decltype(*t++),
                            typename std::iterator_traits<T>::value_type
                        >::value)
            >;
};

SHAREMIND_DEFINE_CONCEPT(OutputIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
            SHAREMIND_REQUIRE_CONCEPTS(Iterator(T)),
            decltype(*t =
                std::declval<typename std::iterator_traits<T>::value_type>()),
            SHAREMIND_REQUIRE(std::is_same<decltype(++t), T &>::value),
            SHAREMIND_REQUIRE(
                    std::is_convertible<decltype(t++), T const &>::value),
            decltype(*t++ =
                std::declval<typename std::iterator_traits<T>::value_type>())
        >;
};

SHAREMIND_DEFINE_CONCEPT(ForwardIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(InputIterator(T)),
                SHAREMIND_REQUIRE_CONCEPTS(DefaultConstructible(T)),
                SHAREMIND_REQUIRE(
                        std::is_convertible<decltype(t++), T const &>::value),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(*t++),
                            typename std::iterator_traits<T>::reference
                        >::value),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            typename std::iterator_traits<T>::reference,
                            typename std::conditional<
                                Models<OutputIterator(T)>::value,
                                typename std::iterator_traits<T>::value_type &,
                                typename std::iterator_traits<T>::value_type
                                    const &
                            >::type
                        >::value)
            >;
};

SHAREMIND_DEFINE_CONCEPT(BidirectionalIterator) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(ForwardIterator(T)),
                SHAREMIND_REQUIRE(std::is_same<decltype(--t), T &>::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<decltype(t--), T const &>::value),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(*t--),
                            typename std::iterator_traits<T>::reference
                        >::value)
            >;
};

SHAREMIND_DEFINE_CONCEPT(RandomAccessIterator) {

    template <typename T, typename DiffT>
    auto check_(T && t, DiffT n) -> ValidTypes<
                SHAREMIND_REQUIRE(std::is_same<decltype(t += n), T &>::value),
                SHAREMIND_REQUIRE(std::is_same<decltype(t -= n), T &>::value),
                SHAREMIND_REQUIRE(std::is_same<decltype(t + n), T>::value),
                SHAREMIND_REQUIRE(std::is_same<decltype(n + t), T>::value),
                SHAREMIND_REQUIRE(std::is_same<decltype(t - n), T>::value),
                SHAREMIND_REQUIRE(std::is_same<decltype(t - t), DiffT>::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<
                            decltype(t[n]),
                            typename std::iterator_traits<T>::reference
                        >::value)
            >;

    template <typename T>
    auto check(T && t) -> ValidTypes<
        SHAREMIND_REQUIRE_CONCEPTS(BidirectionalIterator(T)),
        SHAREMIND_REQUIRE_CONCEPTS(LessThanComparable(T)),
        SHAREMIND_REQUIRE_CONCEPTS(LessOrEqualComparable(T)),
        SHAREMIND_REQUIRE_CONCEPTS(GreaterThanComparable(T)),
        SHAREMIND_REQUIRE_CONCEPTS(GreaterOrEqualComparable(T)),
        decltype(check_(std::forward<T>(t),
                        std::declval<
                          typename std::iterator_traits<T>::difference_type>()))
    >;

};

SHAREMIND_DEFINE_CONCEPT(ValueSwappable) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(Iterator(T)),
                SHAREMIND_REQUIRE_CONCEPTS(Swappable(decltype(*t)))
            >;
};

SHAREMIND_DEFINE_CONCEPT(NullablePointer) {
    template <typename T>
    auto check(T && t) -> ValidTypes<
                SHAREMIND_REQUIRE_CONCEPTS(EqualityComparable(T)),
                SHAREMIND_REQUIRE_CONCEPTS(DefaultConstructible(T)),
                SHAREMIND_REQUIRE_CONCEPTS(CopyConstructible(T)),
                SHAREMIND_REQUIRE_CONCEPTS(CopyAssignable(T)),
                SHAREMIND_REQUIRE_CONCEPTS(Destructible(T)),
                SHAREMIND_REQUIRE_CONCEPTS(Swappable(T &)),
                SHAREMIND_REQUIRE(std::is_convertible<T, bool>::value),
                SHAREMIND_REQUIRE(
                        std::is_same<decltype(t = nullptr), T &>::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<decltype(t != t), bool>::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<decltype(t == nullptr),
                                            bool>::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<decltype(nullptr == t),
                                            bool>::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<decltype(t != nullptr),
                                            bool>::value),
                SHAREMIND_REQUIRE(
                        std::is_convertible<decltype(nullptr != t),
                                            bool>::value)
            >;
};

SHAREMIND_DEFINE_CONCEPT(Hash) {
    template <typename T, typename Key>
    auto check(T && t, Key && key) -> ValidTypes<
                SHAREMIND_REQUIRE(std::is_object<T>::value),
                SHAREMIND_REQUIRE_CONCEPTS(CopyConstructible(T)),
                SHAREMIND_REQUIRE_CONCEPTS(Destructible(T)),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(std::declval<T &>()(std::declval<Key>())),
                            std::size_t
                        >::value),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(std::declval<T&>()(
                                         std::declval<Key const>())),
                            std::size_t
                        >::value),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(std::declval<T const &>()(
                                         std::declval<Key>())),
                            std::size_t
                        >::value),
                SHAREMIND_REQUIRE(
                        std::is_same<
                            decltype(std::declval<T const &>()(
                                         std::declval<Key const>())),
                            std::size_t
                        >::value)
            >;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_CONCEPTS_H */
