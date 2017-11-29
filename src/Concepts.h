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

#ifndef SHAREMIND_CONCEPTS_H
#define SHAREMIND_CONCEPTS_H

#include <cstddef>
#include <sharemind/compiler-support/GccVersion.h>
#include <type_traits>
#include <utility>
#include "TemplateAll.h"
#include "TemplateAny.h"
#include "TemplateNone.h"
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

#define SHAREMIND_REQUIRE_R(r,...) \
    typename ::std::enable_if<(__VA_ARGS__), r>::type

#define SHAREMIND_REQUIRE(...) \
    SHAREMIND_REQUIRE_R(::sharemind::Detail::RequiresConceptResult,__VA_ARGS__)

#define SHAREMIND_REQUIRE_DEFAULTVALUE \
    ::sharemind::Detail::RequiresConceptResult::Succeed

#define SHAREMIND_REQUIRE_CONCEPTS(...) \
    SHAREMIND_REQUIRE(::sharemind::Models<__VA_ARGS__>::value)

#define SHAREMIND_REQUIRE_CONCEPTS_R(r,...) \
    SHAREMIND_REQUIRE_R(r, ::sharemind::Models<__VA_ARGS__>::value)

#define SHAREMIND_REQUIRES(...) \
    SHAREMIND_REQUIRE(__VA_ARGS__) = SHAREMIND_REQUIRE_DEFAULTVALUE

#define SHAREMIND_REQUIRES_CONCEPTS(...) \
    SHAREMIND_REQUIRE_CONCEPTS(__VA_ARGS__) = SHAREMIND_REQUIRE_DEFAULTVALUE

#define SHAREMIND_DEFINE_CONCEPT(name) struct name: ::sharemind::Concept


SHAREMIND_DEFINE_CONCEPT(Same) {
    template <typename A, typename B>
    auto check(A &&, B &&) -> SHAREMIND_REQUIRE(std::is_same<A, B>::value);
};

namespace Detail {
namespace Concepts {

template <typename T> struct UnpointerFunction;
template <typename C, typename ... Ts> struct UnpointerFunction<C(*)(Ts...)>
{ using type = C(Ts...); };

template <typename T>
using NormalizedConceptT =
        typename UnpointerFunction<typename std::decay<T>::type>::type;

} /* namespace Concepts { */
} /* namespace Detail { */


SHAREMIND_DEFINE_CONCEPT(Not) {
    template <typename Cs>
    auto check(Cs &&) -> SHAREMIND_REQUIRE(
                !Detail::ModelsConcept<
                    Detail::Concepts::NormalizedConceptT<Cs>
                >::value);
};

SHAREMIND_DEFINE_CONCEPT(All) {
    template <typename ... Cs>
    auto check(Cs && ...) -> SHAREMIND_REQUIRE(
                TemplateAll<
                    Detail::ModelsConcept<
                        Detail::Concepts::NormalizedConceptT<Cs>
                    >::value...
                >::value);
};

SHAREMIND_DEFINE_CONCEPT(Any) {
    template <typename ... Cs>
    auto check(Cs && ...) -> SHAREMIND_REQUIRE(
                TemplateAny<
                    Detail::ModelsConcept<
                        Detail::Concepts::NormalizedConceptT<Cs>
                    >::value...
                >::value);
};

SHAREMIND_DEFINE_CONCEPT(None) {
    template <typename ... Cs>
    auto check(Cs && ...) -> SHAREMIND_REQUIRE(
                TemplateNone<
                    Detail::ModelsConcept<
                        Detail::Concepts::NormalizedConceptT<Cs>
                    >::value...
                >::value);
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

SHAREMIND_DEFINE_CONCEPT(DecaysTo) {
    template <typename A, typename B>
    auto check(A &&, B &&)
            -> SHAREMIND_REQUIRE_CONCEPTS(Same(typename std::decay<A>::type,B));
};

SHAREMIND_DEFINE_CONCEPT(Callable) {
    template <typename T, typename ... Args>
    auto check(T && t, Args && ... args)
            -> decltype(t(std::forward<Args>(args)...));
};

SHAREMIND_DEFINE_CONCEPT(UnaryPredicate) {
    template <typename T, typename Arg>
    auto check(T && t, Arg && arg) -> SHAREMIND_REQUIRE_CONCEPTS(
                Callable(T, Arg),
                ConvertibleTo(
                    decltype(std::forward<T>(t)(std::forward<Arg>(arg))),
                    bool)
            );
};

SHAREMIND_DEFINE_CONCEPT(BinaryPredicate) {
    template <typename T, typename A, typename B>
    auto check(T && t, A && a, B && b) -> SHAREMIND_REQUIRE_CONCEPTS(
                Callable(T, A, B),
                ConvertibleTo(
                    decltype(std::forward<T>(t)(std::forward<A>(a),
                                                std::forward<B>(b))),
                    bool)
            );
};

SHAREMIND_DEFINE_CONCEPT(Constructible) {
    template <typename T, typename ... Args>
    auto check(T && t, Args && ... args)
            -> SHAREMIND_REQUIRE(std::is_constructible<T, Args...>::value);
};

SHAREMIND_DEFINE_CONCEPT(NothrowConstructible) {
    template <typename T, typename ... Args>
    auto check(T && t, Args && ... args)
            -> SHAREMIND_REQUIRE(
                    std::is_nothrow_constructible<T, Args...>::value);
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
    template <typename Key> struct ConvertibleToKey { operator Key(); };
    template <typename Key> struct ConvertibleToCKey { operator Key const(); };
    template <typename H, typename Key>
    auto check(H &&, Key &&) -> ValidTypes<
                SHAREMIND_REQUIRE(std::is_object<H>::value),
                SHAREMIND_REQUIRE_CONCEPTS(
                    CopyConstructible(H),
                    Destructible(H),
                    /* Check return type of h(u) where h is a (possibly const)
                       value of H and u is an lvalue of type Key: */
                    Same(decltype(std::declval<H &>()(std::declval<Key &>())),
                         std::size_t),
                    Same(decltype(std::declval<H const &>()(
                                      std::declval<Key &>())),
                         std::size_t),
                    /* Check return type of h(u) where h is a (possibly const)
                       value of H and k is a value of a type convertible to
                       (possibly const) Key. */
                    Same(decltype(std::declval<H &>()(
                                      std::declval<ConvertibleToKey<Key> >())),
                         std::size_t),
                    Same(decltype(std::declval<H const &>()(
                                      std::declval<ConvertibleToKey<Key> >())),
                         std::size_t),
                    Same(decltype(std::declval<H &>()(
                                      std::declval<ConvertibleToCKey<Key> >())),
                         std::size_t),
                    Same(decltype(std::declval<H const &>()(
                                      std::declval<ConvertibleToCKey<Key> >())),
                         std::size_t)
                )
            >;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_CONCEPTS_H */
