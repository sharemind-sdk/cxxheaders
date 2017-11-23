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


namespace sharemind {
namespace Detail {
namespace IntegralComparisons {

enum Types { Invalid, Same, SignedUnsigned, UnsignedSigned };

template <Types TYPES>
using TypesConstant = std::integral_constant<Types, TYPES>;

template <typename A, typename B>
using GetTypes =
    typename std::conditional<
        std::is_integral<A>::value && std::is_integral<B>::value,
        typename std::conditional<
            std::is_signed<A>::value == std::is_signed<B>::value,
            TypesConstant<Same>,
            typename std::conditional<
                std::is_signed<A>::value,
                TypesConstant<SignedUnsigned>,
                TypesConstant<UnsignedSigned>
            >::type
        >::type,
        TypesConstant<Invalid>
    >::type;

#define SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(C, op, suNegResult, usNegResult) \
    template <typename A, typename B, Types = GetTypes<A, B>::value> struct C; \
    template <typename A, typename B> struct C<A, B, Same> { \
        constexpr inline static bool test(A a, B b) noexcept { return a op b; }\
    }; \
    template <typename S, typename U> struct C<S, U, SignedUnsigned> { \
        constexpr inline static bool test(S s, U u) noexcept \
        { return (s < 0) ? suNegResult : signedToUnsigned(s) op u; } \
    }; \
    template <typename U, typename S> struct C<U, S, UnsignedSigned> { \
        constexpr inline static bool test(U u, S s) noexcept \
        { return (s < 0) ? usNegResult : u op signedToUnsigned(s); } \
    }
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LT, < , true,  false);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LE, <=, true,  false);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(EQ, ==, false, false);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NE, !=, true,  true);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GE, >=, false, true);
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GT, > , false, true);
#undef SHAREMIND_INTEGRALCOMPARISIONS_DEFINE

} /* namespace IntegralComparisons { */
} /* namespace Detail { */

#define SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Class,C) \
    template <typename A, typename B> \
    constexpr inline bool integral ## Class(A a, B b) noexcept \
    { return Detail::IntegralComparisons::C<A, B>::test(a, b); }
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LessThan, LT)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Less, LT)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(LessEqual, LE)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Equal, EQ)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(NotEqual, NE)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GreaterEqual, GE)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(Greater, GT)
SHAREMIND_INTEGRALCOMPARISIONS_DEFINE(GreaterThan, GT)
#undef SHAREMIND_INTEGRALCOMPARISIONS_DEFINE

} /* namespace Sharemind { */

#endif /* SHAREMIND_INTEGRALCOMPARISIONS_H */
