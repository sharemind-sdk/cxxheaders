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

#include "../src/IdentifierPool.h"

#include <algorithm>
#include <random>
#include "../src/TestAssert.h"


int main() {
    std::random_device rd;
    std::mt19937 g(rd());

    sharemind::IdentifierPool<unsigned> pool;
    std::vector<unsigned> reserved;
    constexpr static unsigned const ADD1 = 1000u;
    constexpr static unsigned const REMOVE1 = 700u; // 300u
    constexpr static unsigned const ADD2 = 900u;    // 1200u
    constexpr static unsigned const REMOVE2 = 300u; // 900u
    constexpr static unsigned const ADD3 = 500u;    // 1400u
    constexpr static unsigned const REMOVE3 = 400u; // 1000u

    reserved.reserve(2000u);
    #define SHUFFLE \
        do { std::shuffle(reserved.begin(), reserved.end(), g); } while (false)
    #define ADD(num) \
        do { \
            auto const n = (num); \
            for (unsigned i = 0u; i < n; ++i) \
                reserved.push_back(pool.reserve()); \
        } while(false);
    #define REMOVE(num) \
        do { \
            auto const n = (num); \
            auto size = reserved.size(); \
            SHAREMIND_TESTASSERT(size >= n); \
            for (unsigned i = 0u; i < n; ++i) { \
                auto const id = reserved[--size]; \
                reserved.pop_back(); \
                pool.recycle(id); \
            } \
        } while (false)

    ADD(ADD1); SHUFFLE; REMOVE(REMOVE1);
    ADD(ADD2); SHUFFLE; REMOVE(REMOVE2);
    ADD(ADD3); SHUFFLE; REMOVE(REMOVE3);
    REMOVE(reserved.size());
}
