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

#include "../src/SimpleUnorderedStringMap.h"

#include <boost/range/iterator_range.hpp>
#include <cstddef>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>
#include "../src/TestAssert.h"


namespace {

struct Rand {
    std::random_device rd;
    std::mt19937 rng{rd()};
    std::uniform_int_distribution<unsigned> dist;

    unsigned get() noexcept { return dist(rng); }
} * myRand;

constexpr std::size_t const ITERATIONS = 100u;
static_assert(ITERATIONS > 0u, "");

template <typename T>
using UM = sharemind::SimpleUnorderedStringMap<T>;

} // anonymous namespace

using H = UM<int>::hasher;
static_assert(std::is_nothrow_default_constructible<H>::value, "");
static_assert(std::is_nothrow_copy_constructible<H>::value, "");
static_assert(std::is_nothrow_move_constructible<H>::value, "");
static_assert(std::is_nothrow_copy_assignable<H>::value, "");
static_assert(std::is_nothrow_move_assignable<H>::value, "");

using P = UM<int>::key_equal;
static_assert(std::is_nothrow_default_constructible<P>::value, "");
static_assert(std::is_nothrow_copy_constructible<P>::value, "");
static_assert(std::is_nothrow_move_constructible<P>::value, "");
static_assert(std::is_nothrow_copy_assignable<P>::value, "");
static_assert(std::is_nothrow_move_assignable<P>::value, "");

int main() {
    myRand = new Rand;
    try {
        using V = decltype(myRand->get());
        UM<V> m;
        static_assert(noexcept(m.hash_function()), "");
        static_assert(noexcept(m.key_eq()), "");
        using VP = decltype(m)::value_type;
        std::vector<VP> valuePairs;
        {
            std::unordered_set<V> used;
            for (std::size_t i = 0u; i < ITERATIONS; ++i) {
                V newValue;
                do {
                    newValue = myRand->get();
                } while (used.find(newValue) != used.end());
                used.emplace(newValue);
                valuePairs.emplace_back(std::to_string(newValue), newValue);
            }
        }

        decltype(m) const & cm = m;
        for (auto const & vp: valuePairs) {
            auto const r(m.emplace(vp));
            SHAREMIND_TESTASSERT(r.second);
            SHAREMIND_TESTASSERT(r.first->first == vp.first);
            SHAREMIND_TESTASSERT(r.first->second == vp.second);
        }

        for (auto const & vp: valuePairs) {
            auto const r(m.emplace(vp.first, vp.second));
            SHAREMIND_TESTASSERT(!r.second);
            SHAREMIND_TESTASSERT(r.first->first == vp.first);
            SHAREMIND_TESTASSERT(r.first->second == vp.second);
        }

        std::string const noSuchValue(
                    "NOT READY READING VALUE! ABORT, RETRY, FAIL?");

        static auto const rangeOf =
                [](std::string & str) noexcept
                { return boost::make_iterator_range(str.begin(), str.end()); };
        static auto const constRangeOf =
                [](std::string const & str) noexcept
                { return boost::make_iterator_range(str.begin(), str.end()); };

        // Find:
        for (auto const & vp: valuePairs) {
            {
                auto const it(m.find(vp.first));
                SHAREMIND_TESTASSERT(it != m.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }{
                auto const it(cm.find(vp.first));
                SHAREMIND_TESTASSERT(it != cm.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }{
                auto const it(m.find(vp.first.c_str()));
                SHAREMIND_TESTASSERT(it != m.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }{
                auto const it(cm.find(vp.first.c_str()));
                SHAREMIND_TESTASSERT(it != cm.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }{
                auto const it(m.find(constRangeOf(vp.first)));
                SHAREMIND_TESTASSERT(it != m.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }{
                auto const it(cm.find(constRangeOf(vp.first)));
                SHAREMIND_TESTASSERT(it != cm.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }{
                auto tmp(vp.first);
                auto const it(m.find(rangeOf(tmp)));
                SHAREMIND_TESTASSERT(it != m.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }{
                auto tmp(vp.first);
                auto const it(cm.find(rangeOf(tmp)));
                SHAREMIND_TESTASSERT(it != cm.end());
                SHAREMIND_TESTASSERT(it->first == vp.first);
                SHAREMIND_TESTASSERT(it->second == vp.second);
            }
        }
        SHAREMIND_TESTASSERT(m.find(noSuchValue) == m.end());
        SHAREMIND_TESTASSERT(cm.find(noSuchValue) == cm.end());
        SHAREMIND_TESTASSERT(m.find(noSuchValue.c_str()) == m.end());
        SHAREMIND_TESTASSERT(cm.find(noSuchValue.c_str()) == cm.end());
        SHAREMIND_TESTASSERT(m.find(constRangeOf(noSuchValue)) == m.end());
        SHAREMIND_TESTASSERT(cm.find(constRangeOf(noSuchValue)) == cm.end());
        {
            auto tmp(noSuchValue);
            SHAREMIND_TESTASSERT(m.find(rangeOf(tmp)) == m.end());
            SHAREMIND_TESTASSERT(cm.find(rangeOf(tmp)) == cm.end());
        }

        // Count:
        for (auto const & vp: valuePairs) {
            SHAREMIND_TESTASSERT(m.count(vp.first) == 1u);
            SHAREMIND_TESTASSERT(cm.count(vp.first) == 1u);
            SHAREMIND_TESTASSERT(m.count(vp.first.c_str()) == 1u);
            SHAREMIND_TESTASSERT(cm.count(vp.first.c_str()) == 1u);
            SHAREMIND_TESTASSERT(m.count(constRangeOf(vp.first)) == 1u);
            SHAREMIND_TESTASSERT(cm.count(constRangeOf(vp.first)) == 1u);
            {
                auto tmp(vp.first);
                SHAREMIND_TESTASSERT(m.count(rangeOf(tmp)) == 1u);
            }{
                auto tmp(vp.first);
                SHAREMIND_TESTASSERT(cm.count(rangeOf(tmp)) == 1u);
            }
        }
        SHAREMIND_TESTASSERT(m.count(noSuchValue) == 0u);
        SHAREMIND_TESTASSERT(cm.count(noSuchValue) == 0u);
        SHAREMIND_TESTASSERT(m.count(noSuchValue.c_str()) == 0u);
        SHAREMIND_TESTASSERT(cm.count(noSuchValue.c_str()) == 0u);
        SHAREMIND_TESTASSERT(m.count(constRangeOf(noSuchValue)) == 0u);
        SHAREMIND_TESTASSERT(cm.count(constRangeOf(noSuchValue)) == 0u);
        {
            auto tmp(noSuchValue);
            SHAREMIND_TESTASSERT(m.count(rangeOf(tmp)) == 0u);
            SHAREMIND_TESTASSERT(cm.count(rangeOf(tmp)) == 0u);
        }

        // Equal range:
        for (auto const & vp: valuePairs) {
            auto const testConst =
                    [&vp](std::pair<UM<V>::const_iterator,
                                    UM<V>::const_iterator> const & p)
                    {
                        SHAREMIND_TESTASSERT(p.first != p.second);
                        SHAREMIND_TESTASSERT(std::next(p.first) == p.second);
                        SHAREMIND_TESTASSERT(vp == *p.first);
                    };
            auto const test =
                    [&vp](std::pair<UM<V>::iterator, UM<V>::iterator> const & p)
                    {
                        SHAREMIND_TESTASSERT(p.first != p.second);
                        SHAREMIND_TESTASSERT(std::next(p.first) == p.second);
                        SHAREMIND_TESTASSERT(vp == *p.first);
                    };
            test(m.equal_range(vp.first));
            testConst(cm.equal_range(vp.first));
            test(m.equal_range(vp.first.c_str()));
            testConst(cm.equal_range(vp.first.c_str()));
            test(m.equal_range(constRangeOf(vp.first)));
            testConst(cm.equal_range(constRangeOf(vp.first)));
            {
                auto tmp(vp.first);
                test(m.equal_range(rangeOf(tmp)));
            }{
                auto tmp(vp.first);
                testConst(cm.equal_range(rangeOf(tmp)));
            }
        }{
            auto const testConst =
                    [&cm](std::pair<UM<V>::const_iterator,
                                    UM<V>::const_iterator> const & p)
                    {
                        SHAREMIND_TESTASSERT(p.first == cm.end());
                        SHAREMIND_TESTASSERT(p.second == cm.end());
                    };
            auto const test =
                    [&m](std::pair<UM<V>::iterator, UM<V>::iterator> const & p)
                    {
                        SHAREMIND_TESTASSERT(p.first == m.end());
                        SHAREMIND_TESTASSERT(p.second == m.end());
                    };
            test(m.equal_range(noSuchValue));
            testConst(cm.equal_range(noSuchValue));
            test(m.equal_range(noSuchValue.c_str()));
            testConst(cm.equal_range(noSuchValue.c_str()));
            test(m.equal_range(constRangeOf(noSuchValue)));
            testConst(cm.equal_range(constRangeOf(noSuchValue)));
            {
                auto tmp(noSuchValue);
                testConst(cm.equal_range(rangeOf(tmp)));
            }{
                auto tmp(noSuchValue);
                testConst(cm.equal_range(rangeOf(tmp)));
            }
        }

        /// \todo Add tests for bucket interface

    } catch (...) {
        delete myRand;
        throw;
    }
    delete myRand;
}
