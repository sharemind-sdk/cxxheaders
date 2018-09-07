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

#include "../src/Optional.h"

#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/for_each_product.hpp>
#include <memory>
#include <type_traits>
#include <utility>
#include "../src/Concepts.h"
#include "../src/TestAssert.h"


#if 0
#include <cstdio>
#define DEBUG_MSG(...) printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...) static_cast<void>(0)
#endif

#define SM sharemind
#define SMO SM::Optional

static_assert(!std::is_default_constructible<SM::NullOption>::value, "");

#define TEST_NULLOPTION_COMPARE(Op,op,r1,r2) \
    template <typename T> \
    constexpr bool testCompare ## Op ## NullOption(SMO<T> const & x) noexcept {\
        static_assert(noexcept(x op SM::nullOption), ""); \
        static_assert(noexcept(SM::nullOption op x), ""); \
        static_assert(std::is_same<decltype(x op SM::nullOption), \
                                   bool>::value, ""); \
        static_assert(std::is_same<decltype(SM::nullOption op x), \
                                   bool>::value, ""); \
        return ((x op SM::nullOption) == r1) && ((SM::nullOption op x) == r2); \
    } \
    static_assert(testCompare ## Op ## NullOption(SMO<int>()), ""); \
    static_assert(testCompare ## Op ## NullOption(SMO<int>(SM::inPlace, 42)),"")
TEST_NULLOPTION_COMPARE(Eq, ==, !x, !x);
TEST_NULLOPTION_COMPARE(Ne, !=, bool(x), bool(x));
TEST_NULLOPTION_COMPARE(Lt, <,  false, bool(x));
TEST_NULLOPTION_COMPARE(Le, <=, !x, true);
TEST_NULLOPTION_COMPARE(Gt, >,  bool(x), false);
TEST_NULLOPTION_COMPARE(Ge, >=, true, !x);
#undef TEST_NULLOPTION_COMPARE

namespace TestCompareWellFormed {
namespace Test { struct A {}; struct B {}; struct Acomp {}; }

#define TEST_COMPARE(Op, op) \
    namespace Test { \
        bool operator op(A const &, Acomp const &) noexcept; \
        bool operator op(Acomp const &, A const &) noexcept; \
    } \
    template <typename T, typename U> \
    constexpr bool test ## Op ## WellFormed() noexcept { \
        return SM::Models<SM::Op ## Comparable(T, U)>::value \
                == SM::Models<SM::Op ## Comparable(SMO<T> const &, \
                                                   SMO<U> const &)>::value; \
    } \
    template <typename T, \
              typename U, \
              SHAREMIND_REQUIRES_CONCEPTS(SM::Op ## Comparable(T, U))> \
    constexpr bool test ## Op ## Noexcept() noexcept { \
        return noexcept(std::declval<T const &>() op std::declval<U const &>())\
               == noexcept(std::declval<SMO<T> const &>() \
                           op std::declval<SMO<U> const &>()); \
    } \
    template <typename T, \
              typename U, \
              SHAREMIND_REQUIRES_CONCEPTS(SM::Not(SM::Op ## Comparable(T, U)))>\
    constexpr bool test ## Op ## Noexcept() noexcept { return true; } \
    template <typename T, typename U> \
    constexpr bool test ## Op() noexcept { \
        return test ## Op ## WellFormed<T, U>() \
               && test ## Op ## Noexcept<T, U>(); \
    } \
    static_assert(test ## Op<Test::A, Test::A>(), "A " #Op " A"); \
    static_assert(test ## Op<Test::A, Test::B>(), "A " #Op " B"); \
    static_assert(test ## Op<Test::B, Test::A>(), "B " #Op " A"); \
    static_assert(test ## Op<Test::A, Test::Acomp>(), "A " #Op " Acomp"); \
    static_assert(test ## Op<Test::Acomp, Test::A>(), "Acomp " #Op " A")
TEST_COMPARE(Equality, ==);
TEST_COMPARE(Inequality, !=);
TEST_COMPARE(LessThan, <);
TEST_COMPARE(LessOrEqual, <=);
TEST_COMPARE(GreaterThan, >);
TEST_COMPARE(GreaterOrEqual, >=);
#undef TEST_COMPARE

} // namespace TestCompareWellFormed {

namespace TestCompareNoexcept {
namespace Test { struct A {}; struct Yes {}; struct No {}; }

#define TEST_COMPARE(op) \
    namespace Test { \
        bool operator op(A const &, Yes const &) noexcept; \
        bool operator op(A const &, No const &); \
    } \
    static_assert(noexcept(std::declval<SMO<Test::A> const &>() \
                           op std::declval<SMO<Test::Yes> const &>()), ""); \
    static_assert(!noexcept(std::declval<SMO<Test::A> const &>() \
                            op std::declval<SMO<Test::No> const &>()), "")
TEST_COMPARE(==);
TEST_COMPARE(!=);
TEST_COMPARE(<);
TEST_COMPARE(<=);
TEST_COMPARE(>);
TEST_COMPARE(>=);
#undef TEST_COMPARE

} // namespace TestCompareNoexcept {

namespace TestCompare {
namespace Test{
#define TEST_CONSTEXPR_STRUCT(name) struct name { constexpr name() {} }
TEST_CONSTEXPR_STRUCT(T);
TEST_CONSTEXPR_STRUCT(TT);
TEST_CONSTEXPR_STRUCT(TF);
TEST_CONSTEXPR_STRUCT(FT);
TEST_CONSTEXPR_STRUCT(FF);
TEST_CONSTEXPR_STRUCT(F);
#undef TEST_CONSTEXPR_STRUCT
};
#define TEST(ns, op, t1, t2, r, ee, ev, ve) \
    namespace Test { \
        constexpr bool operator op(Test::t1 const &, Test::t2 const &) noexcept\
        { return r; } \
    } \
    namespace ns ## _ ## t1 ## _ ## t2 { \
    constexpr SMO<Test::t1> const ot1(SM::inPlace); \
    constexpr SMO<Test::t2> const ot2(SM::inPlace); \
    constexpr SMO<Test::t1> const ot1e; \
    constexpr SMO<Test::t2> const ot2e; \
    constexpr Test::t1 const vt1; \
    constexpr Test::t2 const vt2; \
    static_assert((ot1e op ot2e) == (ee), #t1 " " #op " " #t2 " ee"); \
    static_assert((ot1e op ot2 ) == (ev), #t1 " " #op " " #t2 " ev"); \
    static_assert((ot1  op ot2e) == (ve), #t1 " " #op " " #t2 " ve"); \
    static_assert((ot1  op ot2 ) == (vt1 op vt2), #t1 " " #op " " #t2  " vv"); \
    static_assert((ot1e op vt2 ) == (ev), #t1 " " #op " " #t2  " ev2"); \
    static_assert((vt1  op ot2e) == (ve), #t1 " " #op " " #t2  " ve2"); \
    static_assert((vt1  op ot2 ) == (vt1 op vt2), #t1 " " #op " " #t2  " vv2");\
    static_assert((ot1  op vt2 ) == (vt1 op vt2), #t1 " " #op " " #t2  " vv3");\
    }
#define TEST_COMPARE(ns, op, ee, ev, ve) \
    TEST(ns, op, T, T,  true,  ee, ev, ve) \
    TEST(ns, op, T, TT, true,  ee, ev, ve) \
    TEST(ns, op, TT, T, true,  ee, ev, ve) \
    TEST(ns, op, T, TF, true,  ee, ev, ve) \
    TEST(ns, op, TF, T, false, ee, ev, ve) \
    TEST(ns, op, T, FT, false, ee, ev, ve) \
    TEST(ns, op, FT, T, true,  ee, ev, ve) \
    TEST(ns, op, T, FF, false, ee, ev, ve) \
    TEST(ns, op, FF, T, false, ee, ev, ve) \
    TEST(ns, op, F, F,  false, ee, ev, ve)
TEST_COMPARE(Eq, ==, true,  false, false)
TEST_COMPARE(Ne, !=, false, true,  true)
TEST_COMPARE(Lt, <,  false, true,  false)
TEST_COMPARE(Le, <=, true,  true,  false)
TEST_COMPARE(Gt, >,  false, false, true)
TEST_COMPARE(Ge, >=, true,  false, true)
#undef TEST_COMPARE
#undef TEST

} // namespace TestCompare {

namespace TestCopyMoveDestructTyping {

#define TS (Tr)(Ne)(Re)(De)
#define DTS (Tr)(Ne)
// https://cplusplus.github.io/LWG/lwg-active.html#2827
#define P_LWG2827_Tr(name) !std::is_trivially_destructible<name>::value ||
#define P_LWG2827_Ne(name)
#define P_LWG2827_Re(name)
#define P_LWG2827_De(name)
#define P_Tr std::is_trivially
#define P_Ne std::is_nothrow
#define P_Re !std::is_nothrow
#define P_De !std::is
#define E(name,...) \
    static_assert(__VA_ARGS__<name>::value, #__VA_ARGS__ "<" #name ">")
#define E_CC2(name,...) E(name,__VA_ARGS__ ## _copy_constructible)
#define E_MC2(name,...) E(name,__VA_ARGS__ ## _move_constructible)
#define E_CA2(name,...) E(name,__VA_ARGS__ ## _copy_assignable)
#define E_MA2(name,...) E(name,__VA_ARGS__ ## _move_assignable)
#define E_DS2(name,...) E(name,__VA_ARGS__ ## _destructible)
#define E_CC(name,...) E_CC2(name,__VA_ARGS__)
#define E_MC(name,...) E_MC2(name,__VA_ARGS__)
#define E_CA(name,...) E_CA2(name,__VA_ARGS__)
#define E_MA(name,...) E_MA2(name,__VA_ARGS__)
#define E_DS(name,...) E_DS2(name,__VA_ARGS__)

// Construct test classes:
#define M_Tr(...) = default;
#define M_Ne(...) noexcept __VA_ARGS__
#define M_Re(...) noexcept(false) __VA_ARGS__
#define M_De(...) = delete;
#define DS_Tr(name) ~name() noexcept = default;
#define DS_Ne(name) ~name() noexcept {}
#define DEFINE_MC3(name, CC, MC, CA, MA, DS) \
    struct name { \
        constexpr explicit name(int v) noexcept : m_v(v) {} \
        constexpr name(name const & copy) M_ ## CC(: m_v(copy.m_v + 42) {}) \
        constexpr name(name && move) M_ ## MC(: m_v(move.m_v * move.m_v) {}) \
        name & operator=(name const & copy) \
                M_ ## CA({ m_v = copy.m_v * 13; return *this; }) \
        name & operator=(name && move) \
                M_ ## MA({ m_v = move.m_v * 17; return *this; }) \
        DS_ ## DS(name) \
        int m_v; \
    }; \
    E_CC(name, P_LWG2827_ ## CC(name) P_ ## CC); \
    E_MC(name, P_LWG2827_ ## MC(name) P_ ## MC); \
    E_CA(name, P_ ## CA); \
    E_MA(name, P_ ## MA); \
    E_DS(name, P_ ## DS);
#define DEFINE_MC2(CC, MC, CA, MA, DS) \
    DEFINE_MC3(CC##MC##CA##MA##DS, CC, MC, CA, MA, DS)
#define DEFINE_MC(CC, MC, CA, MA, DS) DEFINE_MC2(CC, MC, CA, MA, DS)
#define D(r, product) \
    DEFINE_MC(BOOST_PP_SEQ_ELEM(0, product), \
              BOOST_PP_SEQ_ELEM(1, product), \
              BOOST_PP_SEQ_ELEM(2, product), \
              BOOST_PP_SEQ_ELEM(3, product), \
              BOOST_PP_SEQ_ELEM(4, product))
BOOST_PP_SEQ_FOR_EACH_PRODUCT(D, (TS)(TS)(TS)(TS)(DTS))
#undef D
#undef DEFINE_MC
#undef DEFINE_MC2
#undef M_De
#undef M_Re
#undef M_Ne
#undef M_Tr

// Test copy constructor:
#define TEST_CC3(CC, MC, CA, MA, DS) \
    E_CC(SM::Optional<CC##MC##CA##MA##DS>, \
         std::is_copy_constructible<CC##MC##CA##MA##DS>::value \
         == std::is); \
    E_CC(SM::Optional<CC##MC##CA##MA##DS>, \
         std::is_nothrow_copy_constructible<CC##MC##CA##MA##DS>::value \
         == std::is_nothrow); \
    E_CC(SM::Optional<CC##MC##CA##MA##DS>, \
         std::is_trivially_copy_constructible<CC##MC##CA##MA##DS>::value \
         == std::is_trivially);
#define TEST_CC2(CC, MC, CA, MA, DS) TEST_CC3(CC, MC, CA, MA, DS)
#define TEST_CC(r, product) \
    TEST_CC2(BOOST_PP_SEQ_ELEM(0, product), \
             BOOST_PP_SEQ_ELEM(1, product), \
             BOOST_PP_SEQ_ELEM(2, product), \
             BOOST_PP_SEQ_ELEM(3, product), \
             BOOST_PP_SEQ_ELEM(4, product))
BOOST_PP_SEQ_FOR_EACH_PRODUCT(TEST_CC, (TS)(TS)(TS)(TS)(DTS))
#undef TEST_CC
#undef TEST_CC2
#undef TEST_CC3
/// \todo Test constexpr of copy constructor

// Test move constructor:
template <typename T> struct RRefWrapper { operator T &&() noexcept; };
#define TEST_MC3(CC, MC, CA, MA, DS) \
    static_assert( \
            std::is_move_constructible<CC##MC##CA##MA##DS>::value \
            == std::is_constructible< \
                        SM::Optional<CC##MC##CA##MA##DS>, \
                        RRefWrapper<CC##MC##CA##MA##DS> &>::value, ""); \
    static_assert( \
            !std::is_move_constructible<CC##MC##CA##MA##DS>::value \
            || (std::is_nothrow_move_constructible<CC##MC##CA##MA##DS>::value \
                == std::is_nothrow_move_constructible< \
                            SM::Optional<CC##MC##CA##MA##DS> >::value), ""); \
    static_assert( \
            !std::is_move_constructible<CC##MC##CA##MA##DS>::value \
            || (std::is_trivially_move_constructible<CC##MC##CA##MA##DS>::value\
                == std::is_trivially_move_constructible< \
                            SM::Optional<CC##MC##CA##MA##DS> >::value), "");
#define TEST_MC2(CC, MC, CA, MA, DS) TEST_MC3(CC, MC, CA, MA, DS)
#define TEST_MC(r, product) \
    TEST_MC2(BOOST_PP_SEQ_ELEM(0, product), \
             BOOST_PP_SEQ_ELEM(1, product), \
             BOOST_PP_SEQ_ELEM(2, product), \
             BOOST_PP_SEQ_ELEM(3, product), \
             BOOST_PP_SEQ_ELEM(4, product))
BOOST_PP_SEQ_FOR_EACH_PRODUCT(TEST_MC, (TS)(TS)(TS)(TS)(DTS))
#undef TEST_MC
#undef TEST_MC2
#undef TEST_MC3
/// \todo Test constexpr of move constructor

// Test copy assignment:
#define TEST_CA3(CC, MC, CA, MA, DS) \
    E_CA(SM::Optional<CC##MC##CA##MA##DS>, \
         (std::is_copy_constructible<CC##MC##CA##MA##DS>::value \
          && std::is_copy_assignable<CC##MC##CA##MA##DS>::value) \
         == std::is); \
    E_CA(SM::Optional<CC##MC##CA##MA##DS>, \
         (std::is_nothrow_copy_constructible<CC##MC##CA##MA##DS>::value \
          && std::is_nothrow_copy_assignable<CC##MC##CA##MA##DS>::value \
          && std::is_nothrow_destructible<CC##MC##CA##MA##DS>::value) \
         == std::is_nothrow); \
    E_CA(SM::Optional<CC##MC##CA##MA##DS>, !std::is_trivially); // can do without?
#define TEST_CA2(CC, MC, CA, MA, DS) TEST_CA3(CC, MC, CA, MA, DS)
#define TEST_CA(r, product) \
    TEST_CA2(BOOST_PP_SEQ_ELEM(0, product), \
             BOOST_PP_SEQ_ELEM(1, product), \
             BOOST_PP_SEQ_ELEM(2, product), \
             BOOST_PP_SEQ_ELEM(3, product), \
             BOOST_PP_SEQ_ELEM(4, product))
BOOST_PP_SEQ_FOR_EACH_PRODUCT(TEST_CA, (TS)(TS)(TS)(TS)(DTS))
#undef TEST_CA
#undef TEST_CA2
#undef TEST_CA3

// Test move assignment:
template <typename T>
struct HasMoveAssignOperator {
    template <typename U, U & (U::*)(U &&)> struct Sfinae {};
    template <typename U> static constexpr bool test(Sfinae<U, &U::operator=> *)
    { return true; }
    template <typename U> static constexpr bool test(...) { return false; }
    static constexpr bool value = test<T>(nullptr);
};
#define TEST_MA3(CC, MC, CA, MA, DS) \
    static_assert( \
            (std::is_move_constructible<CC##MC##CA##MA##DS>::value \
             && std::is_move_assignable<CC##MC##CA##MA##DS>::value) \
            == HasMoveAssignOperator<SM::Optional<CC##MC##CA##MA##DS> >::value,\
            #CC #MC #CA #MA #DS); \
    E_MA(SM::Optional<CC##MC##CA##MA##DS>, \
         !std::is_nothrow_move_constructible<CC##MC##CA##MA##DS>::value \
         || !std::is_nothrow_move_assignable<CC##MC##CA##MA##DS>::value \
         || std::is_nothrow); \
    static_assert(!std::is_trivially_move_assignable< \
                            SM::Optional<CC##MC##CA##MA##DS> >::value, "");
#define TEST_MA2(CC, MC, CA, MA, DS) TEST_MA3(CC, MC, CA, MA, DS)
#define TEST_MA(r, product) \
    TEST_MA2(BOOST_PP_SEQ_ELEM(0, product), \
             BOOST_PP_SEQ_ELEM(1, product), \
             BOOST_PP_SEQ_ELEM(2, product), \
             BOOST_PP_SEQ_ELEM(3, product), \
             BOOST_PP_SEQ_ELEM(4, product))
BOOST_PP_SEQ_FOR_EACH_PRODUCT(TEST_MA, (TS)(TS)(TS)(TS)(DTS))
#undef TEST_MA
#undef TEST_MA2
#undef TEST_MA3

#undef E_MA
#undef E_CA
#undef E_MC
#undef E_CC
#undef E_MA2
#undef E_CA2
#undef E_MC2
#undef E_CC2
#undef E
#undef P_De
#undef P_Re
#undef P_Ne
#undef P_Tr
#undef TS

} // namespace TestCopyMoveDestructTyping {

struct TestStats {
    TestStats() noexcept { ++m_total; }

    TestStats(TestStats && move) noexcept
        : m_copyConstructorCalled(move.m_copyConstructorCalled)
        , m_copyAssignCalls(move.m_copyAssignCalls)
        , m_copiedFrom(move.m_copiedFrom)
        , m_moveConstructorCalled(move.m_moveConstructorCalled)
        , m_moveAssignCalls(move.m_moveAssignCalls)
        , m_movedFrom(move.m_movedFrom)
        , m_destroyed(move.m_destroyed)
    { ++m_total; }

    TestStats(TestStats const & copy) noexcept
        : m_copyConstructorCalled(copy.m_copyConstructorCalled)
        , m_copyAssignCalls(copy.m_copyAssignCalls)
        , m_copiedFrom(copy.m_copiedFrom)
        , m_moveConstructorCalled(copy.m_moveConstructorCalled)
        , m_moveAssignCalls(copy.m_moveAssignCalls)
        , m_movedFrom(copy.m_movedFrom)
        , m_destroyed(copy.m_destroyed)
    { ++m_total; }

    TestStats(bool copyConstructorCalled,
              unsigned copyAssignCalls,
              unsigned copiedFrom,
              bool moveConstructorCalled,
              unsigned moveAssignCalls,
              unsigned movedFrom,
              bool destroyed) noexcept
        : m_copyConstructorCalled(copyConstructorCalled)
        , m_copyAssignCalls(copyAssignCalls)
        , m_copiedFrom(copiedFrom)
        , m_moveConstructorCalled(moveConstructorCalled)
        , m_moveAssignCalls(moveAssignCalls)
        , m_movedFrom(movedFrom)
        , m_destroyed(destroyed)
    { ++m_total; }

    bool operator==(TestStats const & rhs) noexcept {
        return m_copyConstructorCalled == rhs.m_copyConstructorCalled
            && m_copyAssignCalls == rhs.m_copyAssignCalls
            && m_copiedFrom == rhs.m_copiedFrom
            && m_moveConstructorCalled == rhs.m_moveConstructorCalled
            && m_moveAssignCalls == rhs.m_moveAssignCalls
            && m_movedFrom == rhs.m_movedFrom
            && m_destroyed == rhs.m_destroyed;
    }

    static void assumeAndReset(unsigned expected) {
        SHAREMIND_TESTASSERT(m_total == expected);
        m_total = 0u;
    }

    static unsigned m_total;
    bool m_copyConstructorCalled = false;
    unsigned m_copyAssignCalls = 0u;
    unsigned m_copiedFrom = 0u;
    bool m_moveConstructorCalled = false;
    unsigned m_moveAssignCalls = 0u;
    unsigned m_movedFrom = 0u;
    bool m_destroyed = false;
};

unsigned TestStats::m_total = 0u;

template <typename T>
struct TestType {
    template <typename ... Args>
    explicit TestType(std::shared_ptr<TestStats> stats, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : m_stats(std::move(stats))
        , m_value(std::forward<Args>(args)...)
    { DEBUG_MSG("%p construct\n", this); }

    TestType(TestType & copy) = delete;
    TestType(TestType const && move) = delete;

    TestType(TestType && move)
            noexcept(std::is_nothrow_move_constructible<T>::value)
        : m_value(std::move(move.m_value))
    {
        m_stats->m_moveConstructorCalled = true;
        ++(move.m_stats->m_movedFrom);
        DEBUG_MSG("%p move %p\n", this, &move);
    }

    TestType(TestType const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value)
        : m_value(copy.m_value)
    {
        assert(m_stats);
        assert(copy.m_stats);
        m_stats->m_copyConstructorCalled = true; 
        ++(copy.m_stats->m_copiedFrom);
        DEBUG_MSG("%p copy %p\n", this, &copy);
    }

    TestType & operator=(TestType && move)
            noexcept(std::is_nothrow_move_assignable<T>::value)
    {
        DEBUG_MSG("%p =move %p\n", this, &move);
        m_value = std::move(move.m_value);
        ++(m_stats->m_moveAssignCalls);
        ++(move.m_stats->m_movedFrom);
        return *this;
    }

    TestType & operator=(TestType const & copy)
            noexcept(std::is_nothrow_copy_assignable<T>::value)
    {
        DEBUG_MSG("%p =copy %p\n", this, &copy);
        m_value = copy.m_value;
        ++(m_stats->m_copyAssignCalls);
        ++(copy.m_stats->m_copiedFrom);
        return *this;
    }

    ~TestType() noexcept {
        DEBUG_MSG("%p ~destroy\n", this);
        SHAREMIND_TESTASSERT(m_stats);
        SHAREMIND_TESTASSERT(!m_stats->m_destroyed);
        m_stats->m_destroyed = true;
    }

    std::shared_ptr<TestStats> const m_stats{std::make_shared<TestStats>()};
    T m_value;
};

template <typename T>
bool operator==(TestType<T> const & lhs, TestType<T> const & rhs) noexcept
{ return lhs.m_value == rhs.m_value; };

template <typename T>
bool operator==(TestType<T> const & lhs, T const & rhs) noexcept
{ return lhs.m_value == rhs; };

template <typename T>
bool operator==(T const & lhs, TestType<T> const & rhs) noexcept
{ return lhs == rhs.m_value; };


int main() {
    using T = TestType<char>;
    using OT = SMO<T>;
    using OC = SMO<char>;

    static_assert(std::is_trivially_destructible<OC>::value, "");
    static_assert(!std::is_trivially_destructible<OT>::value, "");

    #define TMP_(c) std::make_shared<TestStats>(), c
    #define TMP(c) SM::inPlace, TMP_(c)
    #define GEN(n,c) OT n(TMP(c))
    DEBUG_MSG("Nothing\n");
    SHAREMIND_TESTASSERT(!OT());
    TestStats::assumeAndReset(0u);
    { OT x; SHAREMIND_TESTASSERT(x.value(TMP_('x')) == 'x'); }
    TestStats::assumeAndReset(1u);
    SHAREMIND_TESTASSERT(OT().value(TMP_('x')) == 'x');
    TestStats::assumeAndReset(1u);

    DEBUG_MSG("\nJust\n");
    SHAREMIND_TESTASSERT(OT(TMP('x')));
    TestStats::assumeAndReset(1u);
    SHAREMIND_TESTASSERT(OT(TMP('x'))->m_value == 'x');
    TestStats::assumeAndReset(1u);
    {
        auto const stats(std::make_shared<TestStats>());
        TestStats::assumeAndReset(1u);
        { OT(SM::inPlace, stats, 'x'); }
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(*stats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nCopy construct from nothing\n");
    {
        OT const x;
        TestStats::assumeAndReset(0u);
        OT y(x);
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(!x);
        SHAREMIND_TESTASSERT(!y);
    }
    DEBUG_MSG("\nCopy construct from something\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        OT y(x);
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(xStats == x->m_stats);
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        ++oldXStats->m_copiedFrom;
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
        SHAREMIND_TESTASSERT(x);
        SHAREMIND_TESTASSERT(y);
        SHAREMIND_TESTASSERT(*x == 'x');
        SHAREMIND_TESTASSERT(*y == 'x');
        SHAREMIND_TESTASSERT(*y->m_stats == TestStats(true, 0u, 0u, false, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nMove construct from nothing\n");
    {
        OT x;
        TestStats::assumeAndReset(0u);
        OT y(std::move(x));
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(!x);
        SHAREMIND_TESTASSERT(!y);
    }
    DEBUG_MSG("\nMove construct from something\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        OT y(std::move(x));
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(xStats == x->m_stats);
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        ++oldXStats->m_movedFrom;
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
        SHAREMIND_TESTASSERT(x);
        SHAREMIND_TESTASSERT(y);
        SHAREMIND_TESTASSERT(*y == 'x');
        SHAREMIND_TESTASSERT(*y->m_stats == TestStats(false, 0u, 0u, true, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nMove-assign nothing to nothing\n");
    {
        OT x;
        OT y;
        x = std::move(y);
        SHAREMIND_TESTASSERT(!x);
        SHAREMIND_TESTASSERT(!y);
        TestStats::assumeAndReset(0u);
    }
    DEBUG_MSG("\nMove-assign nothing to something\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        OT y;
        DEBUG_MSG("BEGIN\n");
        x = std::move(y);
        DEBUG_MSG("END\n");
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(!x);
        SHAREMIND_TESTASSERT(!y);
        SHAREMIND_TESTASSERT(xStats.unique());
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        SHAREMIND_TESTASSERT(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
        oldXStats->m_destroyed = true;
        SHAREMIND_TESTASSERT(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(*xStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
    }
    DEBUG_MSG("\nMove-assign something to nothing\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        OT y;
        DEBUG_MSG("BEGIN\n");
        y = std::move(x);
        DEBUG_MSG("END\n");
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(x);
        SHAREMIND_TESTASSERT(y);
        SHAREMIND_TESTASSERT(*y == 'x');
        SHAREMIND_TESTASSERT(xStats == x->m_stats);
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        ++oldXStats->m_movedFrom;
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
        SHAREMIND_TESTASSERT(*y->m_stats == TestStats(false, 0u, 0u, true, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nMove-assign something to something\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        GEN(y,'y');
        TestStats::assumeAndReset(1u);
        auto const yStats(y->m_stats);
        auto const oldYStats(std::make_shared<TestStats>(*yStats));
        TestStats::assumeAndReset(1u);
        DEBUG_MSG("BEGIN\n");
        x = std::move(y);
        DEBUG_MSG("END\n");
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(x);
        SHAREMIND_TESTASSERT(*x == 'y');
        SHAREMIND_TESTASSERT(y);
        SHAREMIND_TESTASSERT(xStats == x->m_stats);
        SHAREMIND_TESTASSERT(yStats == y->m_stats);
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        ++oldXStats->m_moveAssignCalls;
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
        SHAREMIND_TESTASSERT(!(*oldYStats == *yStats));
        ++oldYStats->m_movedFrom;
        SHAREMIND_TESTASSERT(*oldYStats == *yStats);
        TestStats::assumeAndReset(0u);
    }
    DEBUG_MSG("\nCopy-assign nothing to nothing\n");
    {
        OT x;
        OT y;
        x = y;
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(!x);
        SHAREMIND_TESTASSERT(!y);
    }
    DEBUG_MSG("\nCopy-assign nothing to something\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        OT y;
        DEBUG_MSG("BEGIN\n");
        x = y;
        DEBUG_MSG("END\n");
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(!x);
        SHAREMIND_TESTASSERT(!y);
        SHAREMIND_TESTASSERT(xStats.unique());
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        SHAREMIND_TESTASSERT(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
        oldXStats->m_destroyed = true;
        SHAREMIND_TESTASSERT(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(*xStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
    }
    DEBUG_MSG("\nCopy-assign something to nothing\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        OT y;
        DEBUG_MSG("BEGIN\n");
        y = x;
        DEBUG_MSG("END\n");
        TestStats::assumeAndReset(1u);
        SHAREMIND_TESTASSERT(x);
        SHAREMIND_TESTASSERT(y);
        SHAREMIND_TESTASSERT(*y == 'x');
        SHAREMIND_TESTASSERT(xStats == x->m_stats);
        SHAREMIND_TESTASSERT(xStats != y->m_stats);
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        ++oldXStats->m_copiedFrom;
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
        SHAREMIND_TESTASSERT(*y->m_stats == TestStats(true, 0u, 0u, false, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nCopy-assign something to something\n");
    {
        GEN(x,'x');
        TestStats::assumeAndReset(1u);
        auto const xStats(x->m_stats);
        auto const oldXStats(std::make_shared<TestStats>(*xStats));
        TestStats::assumeAndReset(1u);
        GEN(y,'y');
        TestStats::assumeAndReset(1u);
        auto const yStats(y->m_stats);
        auto const oldYStats(std::make_shared<TestStats>(*yStats));
        TestStats::assumeAndReset(1u);
        DEBUG_MSG("BEGIN\n");
        x = y;
        DEBUG_MSG("END\n");
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(x);
        SHAREMIND_TESTASSERT(*x == 'y');
        SHAREMIND_TESTASSERT(y);
        SHAREMIND_TESTASSERT(xStats == x->m_stats);
        SHAREMIND_TESTASSERT(yStats == y->m_stats);
        SHAREMIND_TESTASSERT(!(*oldXStats == *xStats));
        ++oldXStats->m_copyAssignCalls;
        SHAREMIND_TESTASSERT(*oldXStats == *xStats);
        SHAREMIND_TESTASSERT(!(*oldYStats == *yStats));
        ++oldYStats->m_copiedFrom;
        SHAREMIND_TESTASSERT(*oldYStats == *yStats);
        TestStats::assumeAndReset(0u);
    }
    {
        auto x(SM::makeOptional(42));
        static_assert(std::is_same<decltype(x), SMO<int> >::value, "");
        SHAREMIND_TESTASSERT(x);
        SHAREMIND_TESTASSERT(*x == 42);
    }
}
