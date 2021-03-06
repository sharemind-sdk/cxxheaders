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

#include "../src/SharedResourceMap.h"

#include <atomic>
// #include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>
#include "../src/TestAssert.h"


#ifdef __clang__
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

namespace {

constexpr unsigned numThreads = 100u;
constexpr unsigned numIters = 100000u;

template <typename T>
using NC = typename std::remove_const<T>::type;

constexpr auto expectedCount = (numThreads * numIters) / 2u;
std::atomic<NC<decltype(expectedCount)> > count40(0u);
std::atomic<NC<decltype(expectedCount)> > count42(0u);

std::atomic<unsigned> whichNumber(0u);

constexpr std::memory_order relax = std::memory_order_relaxed;

template <typename A>
inline A inc(std::atomic<A> & v) noexcept
{ return v.fetch_add(1u, relax); }

constexpr auto maxConstructions = (numThreads * numIters);
std::atomic<NC<decltype(maxConstructions)> > constructions(0u);

struct SomeClass {
    SomeClass(unsigned const v = 42u) : value(v) { inc(constructions); }
    unsigned value;
    void f(unsigned const expected, std::atomic<unsigned> & cnt) const noexcept
    {
        (void) expected;
        SHAREMIND_TESTASSERT(value == expected);
        inc(cnt);
    }
};

auto const constr = [](unsigned const v){ return new SomeClass(v); };

std::mutex threadsAtStartMutex;
std::condition_variable threadsAtStartCond;
unsigned threadsAtStart = 0u;

std::mutex startMutex;
std::condition_variable startCond;
bool start = false;
bool fail = true;


sharemind::SharedResourceMap<unsigned, SomeClass> map;

} // anonymous namespace

void threadFun() noexcept {
    {
        std::lock_guard<std::mutex> const guard(threadsAtStartMutex);
        ++threadsAtStart;
        threadsAtStartCond.notify_one();
    }
    {
        std::unique_lock<std::mutex> lock(startMutex);
        startCond.wait(lock, [&]() noexcept { return start; });
        if (fail)
            return;
    }
    try {
        for (unsigned i = numIters; i > 0u; --i) {
            bool const is40 = inc(whichNumber) % 2;
            unsigned const number = is40 ? 40u : 42u;
            auto & cnt = is40 ? count40 : count42;
            auto v = map.getResource(number, constr);
            SHAREMIND_TESTASSERT(v);
            v->f(number, cnt);
        }
    } catch (std::bad_alloc const &) {}
}

int main() {
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    {
        auto a = map.getResource(
                    42u,
                    [](unsigned const)
                    { return std::unique_ptr<SomeClass>(new SomeClass(42u)); });
        static_assert(std::is_same<decltype(a),
                                   std::shared_ptr<SomeClass> >::value, "");
        SHAREMIND_TESTASSERT(a);
        SHAREMIND_TESTASSERT(a->value == 42u);
        auto b = map.getResource(42u);
        static_assert(std::is_same<decltype(a), decltype(b)>::value, "");
        SHAREMIND_TESTASSERT(b);
        SHAREMIND_TESTASSERT(b->value == 42u);
        auto c = map.getResource(40u, constr);
        static_assert(std::is_same<decltype(a), decltype(c)>::value, "");
        SHAREMIND_TESTASSERT(c);
        SHAREMIND_TESTASSERT(c->value == 40u);
        SHAREMIND_TESTASSERT(map.size() == 2u);
        SHAREMIND_TESTASSERT(a == b);
        SHAREMIND_TESTASSERT(a != c);
        SHAREMIND_TESTASSERT(b != c);
        b.reset();
        SHAREMIND_TESTASSERT(map.size() == 2u);
        a.reset();
        SHAREMIND_TESTASSERT(map.size() == 1u);
    }
    SHAREMIND_TESTASSERT(map.empty());

    constructions.store(0u, relax);
    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    try {
        for (unsigned i = 0u; i < numThreads; ++i)
            threads.emplace_back(&threadFun);
        {
            std::unique_lock<std::mutex> lock(threadsAtStartMutex);
            threadsAtStartCond.wait(
                        lock,
                        []() noexcept { return threadsAtStart == numThreads; });
        }
        {
            std::lock_guard<std::mutex> const guard(startMutex);
            start = true;
            fail = false;
            startCond.notify_all();
        }
    } catch (...) {
        {
            std::lock_guard<std::mutex> const guard(startMutex);
            start = true;
            startCond.notify_all();
        }
        while (!threads.empty()) {
            (*threads.rbegin()).join();
            threads.pop_back();
        }
        throw;
    }
    while (!threads.empty()) {
        (*threads.rbegin()).join();
        threads.pop_back();
    }
    SHAREMIND_TESTASSERT(map.empty());
    SHAREMIND_TESTASSERT(count40.load(relax) == expectedCount);
    SHAREMIND_TESTASSERT(count42.load(relax) == expectedCount);

    SHAREMIND_TESTASSERT(constructions.load(relax) <= maxConstructions);
    /// \warning The following assertion will fail with very low probability,
    ///          but more so if (numThreads * numIters) is too low.
    static auto const might_fail_with_very_low_probability_or_valgrind
            = [](bool const r) noexcept { return r; };
    SHAREMIND_TESTASSERT(might_fail_with_very_low_probability_or_valgrind(
                             constructions.load(relax) < maxConstructions));

    /// \todo Test forEach()
    /// \todo Test waitForEmpty()
}
