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

#ifndef SHAREMIND_INTEGERLITERALS_H
#define SHAREMIND_INTEGERLITERALS_H

#include <cstdint>
#include <limits>
#include <sharemind/preprocessor.h>
#include <type_traits>


namespace sharemind {
namespace IntegerLiterals {
namespace Detail {

template <std::uintmax_t Max, char ...> struct OctLiteral;
template <std::uintmax_t Max, char ...> struct DecLiteral;
template <std::uintmax_t Max, char ...> struct HexLiteral;

enum LiteralType { Oct, Dec, Hex, Float };

template <std::uintmax_t Base>
struct LiteralBase {
    constexpr static bool const valid = true;
    constexpr static std::uintmax_t const base = Base;
};

struct InvalidDigit { constexpr static bool const valid = false; };

template <std::uintmax_t Base, std::uintmax_t Max, std::uintmax_t Value>
struct LastDigit: LiteralBase<Base> {
    constexpr static bool const overflow = Value > Max;
    constexpr static bool const valid = true;
    constexpr static std::uintmax_t const value = Value;
    constexpr static std::uintmax_t const rank = 1u;
    constexpr static std::uintmax_t const digits = 1u;
};

template <std::uintmax_t Max, std::uintmax_t Value>
using OctDigit = LastDigit<8u, Max, Value>;
template <std::uintmax_t Max, std::uintmax_t Value>
using DecDigit = LastDigit<10u, Max, Value>;
template <std::uintmax_t Max, std::uintmax_t Value>
using HexDigit = LastDigit<16u, Max, Value>;

template <std::uintmax_t Max> struct OctLiteral<Max>: LiteralBase<8u> {
    constexpr static bool const overflow = false;
    constexpr static bool const valid = true;
    constexpr static std::uintmax_t const value = 0u;
    constexpr static std::uintmax_t const digits = 0u;
};
template <std::uintmax_t Max> struct OctLiteral<Max, '0'>: OctDigit<Max, 0u> {};
template <std::uintmax_t Max> struct OctLiteral<Max, '1'>: OctDigit<Max, 1u> {};
template <std::uintmax_t Max> struct OctLiteral<Max, '2'>: OctDigit<Max, 2u> {};
template <std::uintmax_t Max> struct OctLiteral<Max, '3'>: OctDigit<Max, 3u> {};
template <std::uintmax_t Max> struct OctLiteral<Max, '4'>: OctDigit<Max, 4u> {};
template <std::uintmax_t Max> struct OctLiteral<Max, '5'>: OctDigit<Max, 5u> {};
template <std::uintmax_t Max> struct OctLiteral<Max, '6'>: OctDigit<Max, 6u> {};
template <std::uintmax_t Max> struct OctLiteral<Max, '7'>: OctDigit<Max, 7u> {};
template <std::uintmax_t Max, char C> struct OctLiteral<Max, C>: InvalidDigit {};

template <std::uintmax_t Max> struct DecLiteral<Max, '0'>: DecDigit<Max, 0u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '1'>: DecDigit<Max, 1u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '2'>: DecDigit<Max, 2u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '3'>: DecDigit<Max, 3u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '4'>: DecDigit<Max, 4u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '5'>: DecDigit<Max, 5u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '6'>: DecDigit<Max, 6u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '7'>: DecDigit<Max, 7u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '8'>: DecDigit<Max, 8u> {};
template <std::uintmax_t Max> struct DecLiteral<Max, '9'>: DecDigit<Max, 9u> {};
template <std::uintmax_t Max, char C> struct DecLiteral<Max, C>: InvalidDigit {};

template <std::uintmax_t Max> struct HexLiteral<Max, '0'>: HexDigit<Max, 0u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '1'>: HexDigit<Max, 1u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '2'>: HexDigit<Max, 2u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '3'>: HexDigit<Max, 3u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '4'>: HexDigit<Max, 4u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '5'>: HexDigit<Max, 5u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '6'>: HexDigit<Max, 6u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '7'>: HexDigit<Max, 7u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '8'>: HexDigit<Max, 8u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, '9'>: HexDigit<Max, 9u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'a'>: HexDigit<Max, 10u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'b'>: HexDigit<Max, 11u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'c'>: HexDigit<Max, 12u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'd'>: HexDigit<Max, 13u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'e'>: HexDigit<Max, 14u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'f'>: HexDigit<Max, 15u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'A'>: HexDigit<Max, 10u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'B'>: HexDigit<Max, 11u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'C'>: HexDigit<Max, 12u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'D'>: HexDigit<Max, 13u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'E'>: HexDigit<Max, 14u> {};
template <std::uintmax_t Max> struct HexLiteral<Max, 'F'>: HexDigit<Max, 15u> {};
template <std::uintmax_t Max, char C> struct HexLiteral<Max, C>: InvalidDigit {};

template <char ...> struct AreOctDigits: std::true_type {};
template <char C, char ... Cs> struct AreOctDigits<C, Cs...> {
    constexpr static bool const value =
        OctLiteral<0u, C>::valid && AreOctDigits<Cs...>::value;
};

template <char ...> struct AreDecDigits: std::true_type {};
template <char C, char ... Cs> struct AreDecDigits<C, Cs...> {
    constexpr static bool const value =
        DecLiteral<0u, C>::valid && AreDecDigits<Cs...>::value;
};

template <char ...> struct AreHexDigits: std::true_type {};
template <char C, char ... Cs> struct AreHexDigits<C, Cs...> {
    constexpr static bool const value =
        HexLiteral<0u, C>::valid && AreHexDigits<Cs...>::value;
};

template <std::uintmax_t Max, template <std::uintmax_t, char ...> class T, char D, char C, char ... Cs>
struct LiteralStep {
    using Head = T<Max, D>;
    using Tail = T<Max, C, Cs...>;
    static_assert(Head::base == Tail::base, "");
    constexpr static std::uintmax_t const base = Head::base;
    constexpr static bool const overflow =
            Head::overflow
            || Tail::overflow
            || ((Head::value != 0)
                && (
                    (Tail::rank > (Max / base))
                    || (Head::value > (Max / (Tail::rank * base)))
                    || (Tail::value
                        > (Max - (Head::value * (Tail::rank * base))))
                ));
    constexpr static std::uintmax_t const rank = Tail::rank * base;
    constexpr static std::uintmax_t const value = Head::value * rank + Tail::value;
    constexpr static std::uintmax_t const digits = Tail::digits + Head::digits;
};

template <std::uintmax_t Max, char D, char C, char ... Cs>
struct DecLiteral<Max, D, C, Cs...>
    : LiteralStep<Max, DecLiteral, D, C, Cs...>
{};

template <std::uintmax_t Max, char D, char C, char ... Cs>
struct HexLiteral<Max, D, C, Cs...>
    : LiteralStep<Max, HexLiteral, D, C, Cs...>
{};

template <std::uintmax_t Max, char D, char C, char ... Cs>
struct OctLiteral<Max, D, C, Cs...>
    : LiteralStep<Max, OctLiteral, D, C, Cs...>
{};

struct InvalidLiteral {
    constexpr static bool const valid = false;
    constexpr static std::uintmax_t const base = 0u;
    constexpr static bool const overflow = false;
    constexpr static std::uintmax_t const value = 0u;
    constexpr static std::uintmax_t const rank = 0u;
    constexpr static std::uintmax_t const digits = 0u;
};

template <typename T>
struct ValidLiteral: T { constexpr static bool const valid = true; };

template <std::uintmax_t Max, char ... Cs>
struct Literal_: std::conditional<
            AreDecDigits<Cs...>::value,
            ValidLiteral<DecLiteral<Max, Cs...> >,
            InvalidLiteral
        >::type
{};

template <std::uintmax_t Max, char ... Cs>
struct Literal_<Max, '0', 'x', Cs...>
        : std::conditional<
            AreHexDigits<Cs...>::value,
            ValidLiteral<HexLiteral<Max, Cs...> >,
            InvalidLiteral
        >::type
{};

template <std::uintmax_t Max, char ... Cs>
struct Literal_<Max, '0', 'X', Cs...>
        : std::conditional<
            AreHexDigits<Cs...>::value,
            ValidLiteral<HexLiteral<Max, Cs...> >,
            InvalidLiteral
        >::type
{};

template <std::uintmax_t Max, char ... Cs>
struct Literal_<Max, '0', Cs...>
        : std::conditional<
            AreOctDigits<Cs...>::value,
            ValidLiteral<OctLiteral<Max, Cs...> >,
            InvalidLiteral
        >::type
{};

template <std::uintmax_t Max, char ... Cs>
struct Literal: Literal_<Max, Cs...>
{ constexpr static auto const chars = sizeof...(Cs); };

} /* namespace Detail { */
} /* namespace IntegerLiterals { */

#define SHAREMIND_INTEGERLITERAL_NS ::sharemind::IntegerLiterals
#define SHAREMIND_INTEGERLITERAL_NS_WRAP(...) \
    namespace sharemind { namespace IntegerLiterals { __VA_ARGS__ }}

#define SHAREMIND_INTEGERLITERAL_SIMPLE_TEMPLATE(name,intType) \
    template <char ... Cs> \
    using name = \
            ::sharemind::IntegerLiterals::Detail::Literal< \
                    std::numeric_limits<intType>::max(), \
                    Cs...>;

#define SHAREMIND_INTEGERLITERAL_SIGNED_OPS(T,P,C) \
    template <char ... Cs> \
    constexpr T operator "" SHAREMIND_C(_,P)() { \
        using L = C<Cs...>; \
        static_assert(!L::overflow, "Overflow in _" #P " literal!"); \
        return static_cast<T>(L::value); \
    } \
    template <char ... Cs> \
    constexpr T operator "" SHAREMIND_C3(_,P,_overflow)() { \
        using L = C<Cs...>; \
        return static_cast<T>(L::value); \
    }

#define SHAREMIND_INTEGERLITERAL_UNSIGNED_OPS(T,P,C) \
    template <char ... Cs> \
    constexpr T operator "" SHAREMIND_C(_,P)() { \
        using L = C<Cs...>; \
        return static_cast<T>(L::value); \
    } \
    template <char ... Cs> \
    constexpr T operator "" SHAREMIND_C3(_,P,_no_overflow)() { \
        using L = C<Cs...>; \
        static_assert(!L::overflow, \
                      "Overflow in _" #P "_no_overflow literal!"); \
        return static_cast<T>(L::value); \
    }

} /* namespace sharemind { */

#endif /* #ifndef SHAREMIND_INTEGERLITERALS_H */
