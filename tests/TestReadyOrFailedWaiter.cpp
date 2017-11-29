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

#include "../src/ReadyOrFailedWaiter.h"

#include <cstddef>
#include <chrono>
#include <thread>
#include <type_traits>
#include "../src/compiler-support/GccIsNothrowDestructible.h"
#include "../src/TestAssert.h"


constexpr std::size_t numThreads = 10u;
constexpr std::size_t waitMs = 100u;


static_assert(std::is_same<sharemind::ReadyOrFailedWaiter<false>,
                           sharemind::ReadyOrFailedWaiter<> >::value, "");

struct TestFail {};
struct X {};
struct EA1 {
    EA1(X &&) {}
    EA1() = delete;
    EA1(EA1 const &) = delete;
    EA1 & operator=(EA1 const &) = delete;
};
struct EA2 {
    EA2(X &&) {}
    EA2() = delete;
    EA2(EA2 const &) = delete;
    EA2 & operator=(EA2 const &) = delete;
};
struct E { E(EA1 &&, EA2 &&) noexcept {} };

inline void sleepMs() noexcept
{ std::this_thread::sleep_for(std::chrono::milliseconds(waitMs)); }

template <typename Waiter>
struct StaticAssertions
        : std::integral_constant<
            bool,
            noexcept(std::declval<Waiter &>().reset())
            && noexcept(std::declval<Waiter &>().notifyReady())
            && noexcept(std::declval<Waiter &>().notifyFailure())
            && noexcept(std::declval<Waiter &>().waitReady())
            && !noexcept(std::declval<Waiter &>().template waitReady<X>())
            && !noexcept(std::declval<Waiter &>().template waitReady<E>(EA1(X()), EA2(X())))
            && std::is_nothrow_default_constructible<Waiter>::value
            && sharemind::is_nothrow_destructible<Waiter>::value
            && !std::is_copy_constructible<Waiter>::value
            && !std::is_move_constructible<Waiter>::value
            && !std::is_copy_assignable<Waiter>::value
            && !std::is_move_assignable<Waiter>::value
        >
{};

static_assert(StaticAssertions<sharemind::ReadyOrFailedWaiter<> >::value, "");
static_assert(StaticAssertions<sharemind::ReadyOrFailedWaiter<true> >::value, "");
static_assert(StaticAssertions<sharemind::ReadyOrFailedWaiter<false> >::value, "");

inline void testWithTwoThreads() noexcept {
    sharemind::ReadyOrFailedWaiter<> w;
    {
        std::thread t([&]() noexcept { w.notifyReady(); });
        sleepMs();
        SHAREMIND_TESTASSERT(w.waitReady());
        t.join();
    }
    w.reset();
    {
        std::thread t([&]() noexcept { w.notifyFailure(); });
        sleepMs();
        SHAREMIND_TESTASSERT(!w.waitReady());
        t.join();
    }
    w.reset();
    {
        std::thread t([&]() noexcept { w.notifyReady(); });
        sleepMs();
        w.waitReady<E>(EA1(X()), EA2(X()));
        t.join();
    }
    w.reset();
    {
        std::thread t([&]() noexcept { w.notifyFailure(); });
        sleepMs();
        try {
            w.waitReady<E>(EA1(X()), EA2(X()));
            throw TestFail();
        } catch (E const &) {}

        t.join();
    }
    w.reset();
    {
        std::thread t([&]() noexcept { sleepMs(); w.notifyReady(); });
        SHAREMIND_TESTASSERT(w.waitReady());
        t.join();
    }
    w.reset();
    {
        std::thread t([&]() noexcept { sleepMs(); w.notifyFailure(); });
        SHAREMIND_TESTASSERT(!w.waitReady());
        t.join();
    }
}

template <typename NOTIFYLOGIC, typename THREADLOGIC>
void testMultithreaded(NOTIFYLOGIC notifyLogic,
                       THREADLOGIC threadLogic) noexcept
{
    std::thread * threads[numThreads];
    for (std::size_t i = 0u; i < numThreads; ++i)
        threads[i] = new std::thread(threadLogic);
    notifyLogic();
    for (std::size_t i = 0u; i < numThreads; ++i)
        threads[i]->join();
    for (std::size_t i = 0u; i < numThreads; ++i)
        delete threads[i];
}

int main() {
    testWithTwoThreads();
    sharemind::ReadyOrFailedWaiter<true> w;
    testMultithreaded([&w]() noexcept { sleepMs(); w.notifyReady(); },
                      [&w]() noexcept { SHAREMIND_TESTASSERT(w.waitReady()); });
    w.reset();
    testMultithreaded(
                [&w]() noexcept { sleepMs(); w.notifyFailure(); },
                [&w]() noexcept { SHAREMIND_TESTASSERT(!w.waitReady()); });
}
