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

#include "../src/SharedResourceMap.h"

#include <atomic>
#include <cassert>
// #include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>


namespace {

constexpr unsigned const numThreads = 100u;
constexpr unsigned const numIters = 100000u;

template <typename T>
using NC = typename std::remove_const<T>::type;

constexpr auto const expectedCount = (numThreads * numIters) / 2u;
std::atomic<NC<decltype(expectedCount)> > count40(0u);
std::atomic<NC<decltype(expectedCount)> > count42(0u);

std::atomic<unsigned> whichNumber(0u);

constexpr std::memory_order const relax = std::memory_order_relaxed;

template <typename A>
inline A inc(std::atomic<A> & v) noexcept
{ return v.fetch_add(1u, relax); }

struct SomeClass {
    SomeClass(unsigned const v = 42u) : value(v) {}
    unsigned value;
    void f(unsigned const expected, std::atomic<unsigned> & cnt) const noexcept {
        assert(value == expected);
        inc(cnt);
    }
};

constexpr auto const maxConstructions = (numThreads * numIters);
std::atomic<NC<decltype(maxConstructions)> > constructions(0u);

auto const c40 = []{ inc(constructions); return new SomeClass(40u); };
auto const c42 = []{ inc(constructions); return new SomeClass(42u); };

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
            auto const & constr = is40 ? c40 : c42;
            auto v = map.getResource(number, constr);
            assert(v);
            v->f(number, cnt);
        }
    } catch (std::bad_alloc const &) {}
}

int main() {
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    {
        auto a = map.getResource(42u, c42);
        static_assert(std::is_same<decltype(a),
                                   std::shared_ptr<SomeClass> >::value, "");
        assert(a);
        assert(a->value == 42u);
        auto b = map.getResource(42u);
        assert(b);
        assert(b->value == 42u);
        auto c = map.getResource(40u, c40);
        assert(c);
        assert(c->value == 40u);
        assert(map.size() == 2u);
        assert(a == b);
        assert(a != c);
        assert(b != c);
        b.reset();
        assert(map.size() == 2u);
        a.reset();
        assert(map.size() == 1u);
    }
    assert(map.empty());
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
    assert(map.empty());
    assert(count40.load(relax) == expectedCount);
    assert(count42.load(relax) == expectedCount);

    /// \warning This assertion will fail with very low probability:
    assert(constructions.load(relax) < maxConstructions);

    /// \todo Test forEach()
    /// \todo Test waitForEmpty()
}
