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

#include "../src/IntegralComparisons.h"

#include <cstdint>
#include <limits>
#include <type_traits>
#include "../src/TemplateAll.h"
#include "../src/TemplateContainsType.h"


template <typename ... Ts> struct TList;
template <typename T, T ... vs> struct VList;

template <bool ... TESTS>
using TestT =
        typename std::enable_if<
            sharemind::TemplateAll<TESTS...>::value,
            std::true_type
        >::type;

#define DEFINE_OP_AND_TEST(OP,SH) \
    template <typename A, typename B> \
    constexpr inline bool SH(A a, B b) noexcept \
    { return sharemind::integral ## OP(a, b); } \
    template <typename A, typename B, A a, B b> \
    using Test ## SH = TestT<SH(a, b)>; \
    template <typename A, typename B, A a, B b> \
    using TestNot ## SH = TestT<!SH(a, b)>;
DEFINE_OP_AND_TEST(LessThan, LT)
DEFINE_OP_AND_TEST(LessEqual, LE)
DEFINE_OP_AND_TEST(Equal, EQ)
DEFINE_OP_AND_TEST(NotEqual, NE)
DEFINE_OP_AND_TEST(GreaterEqual, GE)
DEFINE_OP_AND_TEST(GreaterThan, GT)
#undef DEFINE_OP_AND_TEST

template <typename T>
constexpr inline T max() noexcept { return std::numeric_limits<T>::max(); }
template <typename T>
constexpr inline T min() noexcept { return std::numeric_limits<T>::min(); }
template <typename T> constexpr inline T val(T v) noexcept { return v; }

template <typename A, typename B, A a, B b>
using TestSameT =
        TestT<
            TestT<LT(a, b) == (a <  b)>::value
            && TestT<LE(a, b) == (a <= b)>::value
            && TestT<EQ(a, b) == (a == b)>::value
            && TestT<NE(a, b) == (a != b)>::value
            && TestT<GE(a, b) == (a >= b)>::value
            && TestT<GT(a, b) == (a >  b)>::value
        >;

template <typename T>
using AroundZeroT = VList<T, -5, -4 -3, -2, -1, 0, 1, 2, 3, 4, 5>;

template <typename T> struct AroundMaxT_ {
    using type = VList<T, max<T>(), max<T>() - 1, max<T>() - 2, max<T>() - 3,
                       max<T>() - 4>;
};
template <> struct AroundMaxT_<bool> { using type = VList<bool, true>; };
template <typename T> using AroundMaxT = typename AroundMaxT_<T>::type;

template <typename T> struct AroundMinT_ {
    using type = VList<T, min<T>(), min<T>() + 1, min<T>() + 2, min<T>() + 3,
                       min<T>() + 4>;
};
template <> struct AroundMinT_<bool> { using type = VList<bool, false>; };
template <typename T> using AroundMinT = typename AroundMinT_<T>::type;


template <typename, typename> struct TestSamesT_;
template <typename T>
struct TestSamesT_<VList<T>, VList<T> > : std::true_type {};
template <typename T, T ... vs>
struct TestSamesT_<VList<T, vs...>, VList<T> > : std::true_type {};
template <typename T, T ... vs>
struct TestSamesT_<VList<T>, VList<T, vs...> > : std::true_type {};
template <typename T, T v1, T ... vs1, T v2, T ... vs2>
struct TestSamesT_<VList<T, v1, vs1...>, VList<T, v2, vs2...> >
        : TestT<
            TestSameT<T, T, v1, v2>::value
            && TestSamesT_<VList<T, v1, vs1...>, VList<T, vs2...> >::value
            && TestSamesT_<VList<T, vs1...>, VList<T, v2, vs2...> >::value
        >
{};

template <typename U>
using TestUnsignedT =
        TestT<
               TestSamesT_<AroundMinT<U>, AroundMinT<U> >::value
            && TestSamesT_<AroundMinT<U>, AroundMaxT<U> >::value
            && TestSamesT_<AroundMaxT<U>, AroundMinT<U> >::value
            && TestSamesT_<AroundMaxT<U>, AroundMaxT<U> >::value
        >;

template <typename S>
using TestSignedT =
        TestT<
               TestSamesT_<AroundMinT<S>, AroundMinT<S> >::value
            && TestSamesT_<AroundMinT<S>, AroundMaxT<S> >::value
            && TestSamesT_<AroundMinT<S>, AroundZeroT<S> >::value
            && TestSamesT_<AroundMaxT<S>, AroundMinT<S> >::value
            && TestSamesT_<AroundMaxT<S>, AroundMaxT<S> >::value
            && TestSamesT_<AroundMaxT<S>, AroundZeroT<S> >::value
            && TestSamesT_<AroundZeroT<S>, AroundMinT<S> >::value
            && TestSamesT_<AroundZeroT<S>, AroundMaxT<S> >::value
            && TestSamesT_<AroundZeroT<S>, AroundZeroT<S> >::value
        >;

template <typename S>
constexpr inline auto S2U(S s) noexcept ->
        typename std::enable_if<
            std::is_signed<S>::value,
            typename std::make_unsigned<S>::type
        >::type
{ return static_cast<typename std::make_unsigned<S>::type>(s); }

template <typename S, typename U, S s, U u>
using TestSuT =
        TestT<
            (s < 0) ?  LT(s, u) : (LT(s, u) == (S2U(s) <  u)),
            (s < 0) ?  LE(s, u) : (LE(s, u) == (S2U(s) <= u)),
            (s < 0) ? !EQ(s, u) : (EQ(s, u) == (S2U(s) == u)),
            (s < 0) ?  NE(s, u) : (NE(s, u) == (S2U(s) != u)),
            (s < 0) ? !GE(s, u) : (GE(s, u) == (S2U(s) >= u)),
            (s < 0) ? !GT(s, u) : (GT(s, u) == (S2U(s) >  u))
        >;

template <typename U, typename S, U u, S s>
using TestUsT =
        TestT<
            (s < 0) ? !LT(u, s) : (LT(u, s) == (u <  S2U(s))),
            (s < 0) ? !LE(u, s) : (LE(u, s) == (u <= S2U(s))),
            (s < 0) ? !EQ(u, s) : (EQ(u, s) == (u == S2U(s))),
            (s < 0) ?  NE(u, s) : (NE(u, s) == (u != S2U(s))),
            (s < 0) ?  GE(u, s) : (GE(u, s) == (u >= S2U(s))),
            (s < 0) ?  GT(u, s) : (GT(u, s) == (u >  S2U(s)))
        >;


template <typename, typename> struct TestMixedT_;
template <typename S, typename U>
struct TestMixedT_<VList<S>, VList<U> > : std::true_type {};
template <typename S, typename U, S ... ss>
struct TestMixedT_<VList<S, ss...>, VList<U> > : std::true_type {};
template <typename S, typename U, U ... us>
struct TestMixedT_<VList<S>, VList<U, us...> > : std::true_type {};
template <typename S, typename U, S s, S ... ss, U u, U ... us>
struct TestMixedT_<VList<S, s, ss...>, VList<U, u, us...> >
        : TestT<
            TestSuT<S, U, s, u>::value,
            TestUsT<U, S, u, s>::value,
            TestMixedT_<VList<S, s, ss...>, VList<U, us...> >::value,
            TestMixedT_<VList<S, ss...>, VList<U, u, us...> >::value
        >
{};

template <typename S, typename U>
using TestMixedT =
        TestT<
            TestMixedT_<AroundMinT<S>, AroundMinT<U> >::value,
            TestMixedT_<AroundMinT<S>, AroundMaxT<U> >::value,
            TestMixedT_<AroundMaxT<S>, AroundMinT<U> >::value,
            TestMixedT_<AroundMaxT<S>, AroundMaxT<U> >::value,
            TestMixedT_<AroundZeroT<S>, AroundMinT<U> >::value,
            TestMixedT_<AroundZeroT<S>, AroundMaxT<U> >::value
        >;

template <typename, typename = TList<> > struct UniqueTList;
template <typename ... ATs> struct UniqueTList<TList<>, TList<ATs...> >
{ using type = TList<ATs...>; };
template <typename T, typename ... Ts, typename ... ATs>
struct UniqueTList<TList<T, Ts...>, TList<ATs...> >
        : std::conditional<
                sharemind::TemplateContainsType<T, ATs...>::value,
                UniqueTList<TList<Ts...>, TList<ATs...> >,
                UniqueTList<TList<Ts...>, TList<ATs..., T> >
            >::type
{};

using STypes_ =
        TList<
            signed char,
            signed short,
            signed int,
            signed long,
            signed long long,
            std::int8_t,
            std::int16_t,
            std::int32_t,
            std::int64_t,
            std::int_least8_t,
            std::int_least16_t,
            std::int_least32_t,
            std::int_least64_t,
            std::int_fast8_t,
            std::int_fast16_t,
            std::int_fast32_t,
            std::int_fast64_t,
            std::intmax_t,
            std::intptr_t
        >;
using STypes = typename UniqueTList<STypes_>::type;

using UTypes_ =
        TList<
            bool,
            unsigned char,
            unsigned short,
            unsigned int,
            unsigned long,
            unsigned long long,
            std::uint8_t,
            std::uint16_t,
            std::uint32_t,
            std::uint64_t,
            std::uint_least8_t,
            std::uint_least16_t,
            std::uint_least32_t,
            std::uint_least64_t,
            std::uint_fast8_t,
            std::uint_fast16_t,
            std::uint_fast32_t,
            std::uint_fast64_t,
            std::uintmax_t,
            std::uintptr_t
        >;
using UTypes = typename UniqueTList<UTypes_>::type;

template <typename> struct DoTestSigned;
template <typename ... Ts> struct DoTestSigned<TList<Ts...> >
        : TestT<TestSignedT<Ts>::value...>
{};
static_assert(DoTestSigned<STypes>::value, "");

template <typename> struct DoTestUnsigned;
template <typename ... Ts> struct DoTestUnsigned<TList<Ts...> >
        : TestT<TestUnsignedT<Ts>::value...>
{};
static_assert(DoTestUnsigned<UTypes>::value, "");

template <typename, typename> struct DoTestMixed;
template <> struct DoTestMixed<TList<>, TList<> > : std::true_type {};
template <typename ... Ss> struct DoTestMixed<TList<Ss...>, TList<> >
         : std::true_type
{};
template <typename ... Us> struct DoTestMixed<TList<>, TList<Us...> >
        : std::true_type
{};
template <typename S, typename ... Ss, typename U, typename ... Us>
struct DoTestMixed<TList<S, Ss...>, TList<U, Us...> >
        : TestT<
            TestMixedT<S, U>::value
            && DoTestMixed<TList<S, Ss...>, TList<Us...> >::value
            && DoTestMixed<TList<Ss...>, TList<U, Us...> >::value
        >
{};
static_assert(DoTestMixed<STypes, UTypes>::value, "");

template <typename Types, template <typename T> class Test> struct DoUnaryTest;
template <typename ... Ts, template <typename T> class Test>
struct DoUnaryTest<TList<Ts...>, Test>
        : sharemind::TemplateAll<Test<Ts>::value...>
{};

#define DO_SIGNED_TEST(op, nt, zt, pt) \
    template <typename T> \
    using DoSignedTest ## op = \
            sharemind::TemplateAll< \
                sharemind::integral ## op(static_cast<T>(-42)) == nt, \
                sharemind::integral ## op(static_cast<T>(0)) == zt, \
                sharemind::integral ## op(static_cast<T>(42)) == pt \
            >; \
    static_assert(DoUnaryTest<STypes, DoSignedTest ## op>::value, "");
DO_SIGNED_TEST(Positive, false, false, true);
DO_SIGNED_TEST(Negative, true, false, false);
DO_SIGNED_TEST(NonNegative, false, true, true);
DO_SIGNED_TEST(NonPositive, true, true, false);
DO_SIGNED_TEST(Zero, false, true, false);
DO_SIGNED_TEST(NonZero, true, false, true);
#undef DO_SIGNED_TEST

#define DO_UNSIGNED_TEST(op, zt, pt) \
    template <typename T> \
    using DoUnsignedTest ## op = \
            sharemind::TemplateAll< \
                sharemind::integral ## op(static_cast<T>(0u)) == zt, \
                sharemind::integral ## op(static_cast<T>(42u)) == pt \
            >; \
    static_assert(DoUnaryTest<UTypes, DoUnsignedTest ## op>::value, "");
DO_UNSIGNED_TEST(Positive, false, true);
DO_UNSIGNED_TEST(Negative, false, false);
DO_UNSIGNED_TEST(NonNegative, true, true);
DO_UNSIGNED_TEST(NonPositive, true, false);
DO_UNSIGNED_TEST(Zero, true, false);
DO_UNSIGNED_TEST(NonZero, false, true);
#undef DO_UNSIGNED_TEST

int main() {}
