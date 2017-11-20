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
            }
        }
    } catch (...) {
        delete myRand;
        throw;
    }
    delete myRand;
}
