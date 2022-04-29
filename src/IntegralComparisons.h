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

#ifndef SHAREMIND_INTEGRALCOMPARISIONS_H
#define SHAREMIND_INTEGRALCOMPARISIONS_H

#include <type_traits>
#include "SignedToUnsigned.h"

#if __cplusplus >= 202002L
#define SHAREMIND_INTEGRALCOMPARISIONS_DEPRECATED(f,a) \
    [[deprecated(#f " is deprecated, please use " #a " instead!")]]
#else
#define SHAREMIND_INTEGRALCOMPARISIONS_DEPRECATED(f,a)
#endif

namespace sharemind {
namespace Detail {
namespace IntegralComparisons {

enum class Type { Invalid, Signed, Unsigned };

template <Type TYPE>
using TypeConstant = std::integral_constant<Type, TYPE>;

template <typename T>
using GetType =
    typename std::conditional<
        std::is_integral<T>::value,
        typename std::conditional<
            std::is_signed<T>::value,
            TypeConstant<Type::Signed>,
            TypeConstant<Type::Unsigned>
        >::type,
        TypeConstant<Type::Invalid>
    >::type;

enum class Types { Invalid, Same, SignedUnsigned, UnsignedSigned };

template <Types TYPES>
using TypesConstant = std::integral_constant<Types, TYPES>;

template <typename A, typename B>
using GetTypes =
    typename std::conditional<
        std::is_integral<A>::value && std::is_integral<B>::value,
        typename std::conditional<
            std::is_signed<A>::value == std::is_signed<B>::value,
            TypesConstant<Types::Same>,
            typename std::conditional<
                std::is_signed<A>::value,
                TypesConstant<Types::SignedUnsigned>,
                TypesConstant<Types::UnsignedSigned>
            >::type
        >::type,
        TypesConstant<Types::Invalid>
    >::type;

#define SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(C, op, suNegResult, usNegResult) \
    template <typename A, typename B, Types = GetTypes<A, B>::value> struct C; \
    template <typename A, typename B> struct C<A, B, Types::Same> { \
        constexpr static bool test(A a, B b) noexcept { return a op b; } \
    }; \
    template <typename S, typename U> struct C<S, U, Types::SignedUnsigned> { \
        constexpr static bool test(S s, U u) noexcept \
        { return (s < 0) ? suNegResult : signedToUnsigned(s) op u; } \
    }; \
    template <typename U, typename S> struct C<U, S, Types::UnsignedSigned> { \
        constexpr static bool test(U u, S s) noexcept \
        { return (s < 0) ? usNegResult : u op signedToUnsigned(s); } \
    }
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LT, < , true,  false);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LE, <=, true,  false);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(EQ, ==, false, false);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NE, !=, true,  true);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GE, >=, false, true);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GT, > , false, true);
#undef SHAREMIND_INTEGRALCOMPARISIONS_DEFINE
#define SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(C, sop, ...) \
    template <typename T, Type = GetType<T>::value> struct C; \
    template <typename S> struct C<S, Type::Signed> { \
        constexpr static bool test(S s) noexcept { sop; } \
    }; \
    template <typename U> struct C<U, Type::Unsigned> { \
        constexpr static bool test(U u) noexcept { __VA_ARGS__; } \
    }
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Positive,
                                      return s > 0,
                                      return u > 0u);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Negative,
                                      return s < 0,
                                      return (static_cast<void>(u), false));
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NonNegative,
                                      return s >= 0,
                                      return (static_cast<void>(u), true));
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NonPositive,
                                      return s <= 0,
                                      return u <= 0u);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Zero,
                                      return s == 0,
                                      return u == 0u);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NonZero,
                                      return s != 0,
                                      return u != 0u);
#undef SHAREMIND_INTEGRALCOMPARISIONS_DEFINE

} /* namespace IntegralComparisons { */
} /* namespace Detail { */

#define SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Class,C,alt) \
    template <typename A, typename B> \
    SHAREMIND_INTEGRALCOMPARISIONS_DEPRECATED(integral ## Class,alt) \
    constexpr inline bool integral ## Class(A a, B b) noexcept \
    { return Detail::IntegralComparisons::C<A, B>::test(a, b); }
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LessThan, LT, std::cmp_less)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Less, LT, std::cmp_less)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LessEqual, LE, std::cmp_less_equal)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Equal, EQ, std::cmp_equal)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NotEqual, NE, std::cmp_not_equal)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GreaterEqual, GE, std::cmp_greater_equal)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Greater, GT, std::cmp_greater)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GreaterThan, GT, std::cmp_greater)
#undef SHAREMIND_INTEGRALCOMPARISIONS_DEFINE
#define SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(C) \
    template <typename T> \
    constexpr inline bool integral ## C(T t) noexcept \
    { return Detail::IntegralComparisons::C<T>::test(t); }
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Positive)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Negative)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NonNegative)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NonPositive)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Zero)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NonZero)
#undef SHAREMIND_INTEGRALCOMPARISIONS_DEFINE

} /* namespace Sharemind { */

#endif /* SHAREMIND_INTEGRALCOMPARISIONS_H */
