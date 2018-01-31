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

#include <type_traits>
#include "../src/TestAssert.h"


#if 0
#include <cstdio>
#define DEBUG_MSG(...) printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...) static_cast<void>(0)
#endif

using namespace sharemind;

static_assert(!std::is_default_constructible<NullOption>::value, "");

#define TEST_NULLOPTION_COMPARE(Op,op,r1,r2) \
    template <typename T> \
    constexpr bool testCompare ## Op ## NullOption(Optional<T> const & x) \
            noexcept \
    { \
        static_assert(noexcept(x op nullOption), ""); \
        static_assert(noexcept(nullOption op x), ""); \
        static_assert(std::is_same<decltype(x op nullOption), bool>::value, ""); \
        static_assert(std::is_same<decltype(nullOption op x), bool>::value, ""); \
        return ((x op nullOption) == r1) && ((nullOption op x) == r2); \
    } \
    static_assert(testCompare ## Op ## NullOption(Optional<int>()), ""); \
    static_assert(testCompare ## Op ## NullOption(Optional<int>(inPlace, 42)), "")
TEST_NULLOPTION_COMPARE(Eq, ==, !x, !x);
TEST_NULLOPTION_COMPARE(Ne, !=, bool(x), bool(x));
TEST_NULLOPTION_COMPARE(Lt, <,  false, bool(x));
TEST_NULLOPTION_COMPARE(Le, <=, !x, true);
TEST_NULLOPTION_COMPARE(Gt, >,  bool(x), false);
TEST_NULLOPTION_COMPARE(Ge, >=, true, !x);
#undef TEST_NULLOPTION_COMPARE

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
        m_stats.reset();
        SHAREMIND_TESTASSERT(!m_stats);
    }

    std::shared_ptr<TestStats> m_stats{std::make_shared<TestStats>()};
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
    using OT = Optional<T>;
    using OC = Optional<char>;

    static_assert(std::is_trivially_destructible<OC>::value, "");
    static_assert(!std::is_trivially_destructible<OT>::value, "");

    #define TMP_(c) std::make_shared<TestStats>(), c
    #define TMP(c) inPlace, TMP_(c)
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
        { OT(inPlace, stats, 'x'); }
        TestStats::assumeAndReset(0u);
        SHAREMIND_TESTASSERT(*stats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nCopy construct from nothing\n");
    {
        OT x;
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
}
