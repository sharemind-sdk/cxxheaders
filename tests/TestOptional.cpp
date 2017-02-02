/*
 * Copyright (C) 2017 Cybernetica
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


#if 0
#include <cstdio>
#define DEBUG_MSG(...) printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...) static_cast<void>(0)
#endif

using sharemind::inPlace;
using sharemind::Optional;

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
        assert(m_total == expected);
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
        assert(m_stats);
        assert(!m_stats->m_destroyed);
        m_stats->m_destroyed = true;
        m_stats.reset();
        assert(!m_stats);
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
    assert(!OT());
    TestStats::assumeAndReset(0u);
    { OT x; assert(x.value(TMP_('x')) == 'x'); }
    TestStats::assumeAndReset(1u);
    assert(OT().value(TMP_('x')) == 'x');
    TestStats::assumeAndReset(1u);

    DEBUG_MSG("\nJust\n");
    assert(OT(TMP('x')));
    TestStats::assumeAndReset(1u);
    assert(OT(TMP('x'))->m_value == 'x');
    TestStats::assumeAndReset(1u);
    {
        auto const stats(std::make_shared<TestStats>());
        TestStats::assumeAndReset(1u);
        { OT(inPlace, stats, 'x'); }
        TestStats::assumeAndReset(0u);
        assert(*stats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nCopy construct from nothing\n");
    {
        OT x;
        TestStats::assumeAndReset(0u);
        OT y(x);
        TestStats::assumeAndReset(0u);
        assert(!x);
        assert(!y);
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
        assert(xStats == x->m_stats);
        assert(!(*oldXStats == *xStats));
        ++oldXStats->m_copiedFrom;
        assert(*oldXStats == *xStats);
        assert(x);
        assert(y);
        assert(*x == 'x');
        assert(*y == 'x');
        assert(*y->m_stats == TestStats(true, 0u, 0u, false, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nMove construct from nothing\n");
    {
        OT x;
        TestStats::assumeAndReset(0u);
        OT y(std::move(x));
        TestStats::assumeAndReset(0u);
        assert(!x);
        assert(!y);
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
        assert(xStats == x->m_stats);
        assert(!(*oldXStats == *xStats));
        ++oldXStats->m_movedFrom;
        assert(*oldXStats == *xStats);
        assert(x);
        assert(y);
        assert(*y == 'x');
        assert(*y->m_stats == TestStats(false, 0u, 0u, true, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
    }
    DEBUG_MSG("\nMove-assign nothing to nothing\n");
    {
        OT x;
        OT y;
        x = std::move(y);
        assert(!x);
        assert(!y);
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
        assert(!x);
        assert(!y);
        assert(xStats.unique());
        assert(!(*oldXStats == *xStats));
        assert(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
        oldXStats->m_destroyed = true;
        assert(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        assert(*xStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        assert(*oldXStats == *xStats);
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
        assert(x);
        assert(y);
        assert(*y == 'x');
        assert(xStats == x->m_stats);
        assert(!(*oldXStats == *xStats));
        ++oldXStats->m_movedFrom;
        assert(*oldXStats == *xStats);
        assert(*y->m_stats == TestStats(false, 0u, 0u, true, 0u, 0u, false));
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
        assert(x);
        assert(*x == 'y');
        assert(y);
        assert(xStats == x->m_stats);
        assert(yStats == y->m_stats);
        assert(!(*oldXStats == *xStats));
        ++oldXStats->m_moveAssignCalls;
        assert(*oldXStats == *xStats);
        assert(!(*oldYStats == *yStats));
        ++oldYStats->m_movedFrom;
        assert(*oldYStats == *yStats);
        TestStats::assumeAndReset(0u);
    }
    DEBUG_MSG("\nCopy-assign nothing to nothing\n");
    {
        OT x;
        OT y;
        x = y;
        TestStats::assumeAndReset(0u);
        assert(!x);
        assert(!y);
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
        assert(!x);
        assert(!y);
        assert(xStats.unique());
        assert(!(*oldXStats == *xStats));
        assert(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, false));
        TestStats::assumeAndReset(1u);
        oldXStats->m_destroyed = true;
        assert(*oldXStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        assert(*xStats == TestStats(false, 0u, 0u, false, 0u, 0u, true));
        TestStats::assumeAndReset(1u);
        assert(*oldXStats == *xStats);
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
        assert(x);
        assert(y);
        assert(*y == 'x');
        assert(xStats == x->m_stats);
        assert(xStats != y->m_stats);
        assert(!(*oldXStats == *xStats));
        ++oldXStats->m_copiedFrom;
        assert(*oldXStats == *xStats);
        assert(*y->m_stats == TestStats(true, 0u, 0u, false, 0u, 0u, false));
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
        assert(x);
        assert(*x == 'y');
        assert(y);
        assert(xStats == x->m_stats);
        assert(yStats == y->m_stats);
        assert(!(*oldXStats == *xStats));
        ++oldXStats->m_copyAssignCalls;
        assert(*oldXStats == *xStats);
        assert(!(*oldYStats == *yStats));
        ++oldYStats->m_copiedFrom;
        assert(*oldYStats == *yStats);
        TestStats::assumeAndReset(0u);
    }
}
