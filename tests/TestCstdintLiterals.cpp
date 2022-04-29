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

#include "../src/CstdintLiterals.h"

#include <cstdint>
#include <limits>
#include <type_traits>


namespace sharemind {

#define TEST1(suffix,type) \
    static_assert(std::is_same<decltype(0_##suffix), std::type##_t>::value, \
                  ""); \
    static_assert(0_##suffix == 0, ""); \
    static_assert(42_##suffix == 42, "");
#define TEST(suffix,type) TEST1(suffix,type) TEST1(u ## suffix, u ## type)

TEST(int8,int8)
TEST(int16,int16)
TEST(int32,int32)
TEST(int64,int64)
TEST(intFast8,int_fast8)
TEST(intFast16,int_fast16)
TEST(intFast32,int_fast32)
TEST(intFast64,int_fast64)
TEST(intLeast8,int_least8)
TEST(intLeast16,int_least16)
TEST(intLeast32,int_least32)
TEST(intLeast64,int_least64)
TEST(intMax,intmax)
TEST(intPtr,intptr)
#define TESTMAX(v,T) \
    static_assert( \
            0x ## v ## _ ## T == std::numeric_limits<std::T ## _t>::max(), "");
TESTMAX(7f,int8)
TESTMAX(7fff,int16)
TESTMAX(7fffffff,int32)
TESTMAX(7fffffffffffffff,int64)
TESTMAX(ff,uint8)
TESTMAX(ffff,uint16)
TESTMAX(ffffffff,uint32)
TESTMAX(ffffffffffffffff,uint64)
}

int main() {}
