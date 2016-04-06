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

#include "../src/IntegerLiterals.h"


template <std::uintmax_t Max, char ... Cs>
using L = sharemind::IntegerLiterals::Detail::Literal<Max, Cs...>;

template <char ... Cs> using L13 = L<13u, Cs...>;

template <char ... Cs>
constexpr L13<Cs...> operator "" _13() { return L13<Cs...>(); }

static_assert(!decltype(.0_13)::valid, "");
static_assert(!decltype(0._13)::valid, "");
static_assert(!decltype(0.0_13)::valid, "");
static_assert(!decltype(.000_13)::valid, "");
static_assert(!decltype(000._13)::valid, "");
static_assert(!decltype(0.000_13)::valid, "");
static_assert(!decltype(000.0_13)::valid, "");
static_assert(!decltype(00000.000000_13)::valid, "");
static_assert(!decltype(.0001_13)::valid, "");
static_assert(!decltype(0001._13)::valid, "");
static_assert(!decltype(.1000_13)::valid, "");
static_assert(!decltype(1000._13)::valid, "");
static_assert(!decltype(0000E0000_13)::valid, "");
static_assert(!decltype(0000e0000_13)::valid, "");
static_assert(!decltype(1234E0000_13)::valid, "");
static_assert(!decltype(1234e0000_13)::valid, "");
static_assert(!decltype(0000E-0000_13)::valid, "");
static_assert(!decltype(0000e-0000_13)::valid, "");
static_assert(!decltype(1234E-0000_13)::valid, "");
static_assert(!decltype(1234e-0000_13)::valid, "");
static_assert(!decltype(0000E+0000_13)::valid, "");
static_assert(!decltype(0000e+0000_13)::valid, "");
static_assert(!decltype(1234E+0000_13)::valid, "");
static_assert(!decltype(1234e+0000_13)::valid, "");
static_assert(!decltype(0000E+0000_13)::valid, "");
static_assert(!decltype(0000e+0000_13)::valid, "");
static_assert(!decltype(1234E+0000_13)::valid, "");
static_assert(!decltype(1234e+0000_13)::valid, "");

static_assert(decltype(0_13)::valid, "");
static_assert(decltype(1234567890_13)::valid, "");
static_assert(decltype(01234567_13)::valid, "");
static_assert(decltype(0001234567_13)::valid, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_13)::valid, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_13)::valid, "");
static_assert(decltype(0x00001_13)::valid, "");
static_assert(decltype(0X00001_13)::valid, "");

static_assert(decltype(0_13)::base == 8u, "");
static_assert(decltype(1234567890_13)::base == 10u, "");
static_assert(decltype(01234567_13)::base == 8u, "");
static_assert(decltype(0001234567_13)::base == 8u, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_13)::base == 16u, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_13)::base == 16u, "");
static_assert(decltype(0x00001_13)::base == 16u, "");
static_assert(decltype(0X00001_13)::base == 16u, "");

static_assert(decltype(0_13)::digits == 0u, "");
static_assert(decltype(1234567890_13)::digits == 10u, "");
static_assert(decltype(01234567_13)::digits == 7u, "");
static_assert(decltype(0001234567_13)::digits == 9u, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_13)::digits == 22u, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_13)::digits == 22u, "");
static_assert(decltype(0x00001_13)::digits == 5u, "");
static_assert(decltype(0X00001_13)::digits == 5u, "");

static_assert(decltype(0_13)::chars == 1u, "");
static_assert(decltype(1234567890_13)::chars == 10u, "");
static_assert(decltype(01234567_13)::chars == 8u, "");
static_assert(decltype(0001234567_13)::chars == 10u, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_13)::chars == 24u, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_13)::chars == 24u, "");
static_assert(decltype(0x00001_13)::chars == 7u, "");
static_assert(decltype(0X00001_13)::chars == 7u, "");

static_assert(!decltype(0_13)::overflow, "");
static_assert(!decltype(01_13)::overflow, "");
static_assert(!decltype(02_13)::overflow, "");
static_assert(!decltype(03_13)::overflow, "");
static_assert(!decltype(04_13)::overflow, "");
static_assert(!decltype(05_13)::overflow, "");
static_assert(!decltype(06_13)::overflow, "");
static_assert(!decltype(07_13)::overflow, "");
static_assert(!decltype(010_13)::overflow, "");
static_assert(!decltype(011_13)::overflow, "");
static_assert(!decltype(012_13)::overflow, "");
static_assert(!decltype(013_13)::overflow, "");
static_assert(!decltype(014_13)::overflow, "");
static_assert(!decltype(015_13)::overflow, "");
static_assert(decltype(016_13)::overflow, "");
static_assert(decltype(0_13)::value == 0u, "");
static_assert(decltype(01_13)::value == 1u, "");
static_assert(decltype(02_13)::value == 2u, "");
static_assert(decltype(03_13)::value == 3u, "");
static_assert(decltype(04_13)::value == 4u, "");
static_assert(decltype(05_13)::value == 5u, "");
static_assert(decltype(06_13)::value == 6u, "");
static_assert(decltype(07_13)::value == 7u, "");
static_assert(decltype(010_13)::value == 8u, "");
static_assert(decltype(011_13)::value == 9u, "");
static_assert(decltype(012_13)::value == 10u, "");
static_assert(decltype(013_13)::value == 11u, "");
static_assert(decltype(014_13)::value == 12u, "");
static_assert(decltype(015_13)::value == 13u, "");
static_assert(!decltype(00_13)::overflow, "");
static_assert(!decltype(001_13)::overflow, "");
static_assert(!decltype(002_13)::overflow, "");
static_assert(!decltype(003_13)::overflow, "");
static_assert(!decltype(004_13)::overflow, "");
static_assert(!decltype(005_13)::overflow, "");
static_assert(!decltype(006_13)::overflow, "");
static_assert(!decltype(007_13)::overflow, "");
static_assert(!decltype(0010_13)::overflow, "");
static_assert(!decltype(0011_13)::overflow, "");
static_assert(!decltype(0012_13)::overflow, "");
static_assert(!decltype(0013_13)::overflow, "");
static_assert(!decltype(0014_13)::overflow, "");
static_assert(!decltype(0015_13)::overflow, "");
static_assert(decltype(0016_13)::overflow, "");
static_assert(decltype(00_13)::value == 0u, "");
static_assert(decltype(001_13)::value == 1u, "");
static_assert(decltype(002_13)::value == 2u, "");
static_assert(decltype(003_13)::value == 3u, "");
static_assert(decltype(004_13)::value == 4u, "");
static_assert(decltype(005_13)::value == 5u, "");
static_assert(decltype(006_13)::value == 6u, "");
static_assert(decltype(007_13)::value == 7u, "");
static_assert(decltype(0010_13)::value == 8u, "");
static_assert(decltype(0011_13)::value == 9u, "");
static_assert(decltype(0012_13)::value == 10u, "");
static_assert(decltype(0013_13)::value == 11u, "");
static_assert(decltype(0014_13)::value == 12u, "");
static_assert(decltype(0015_13)::value == 13u, "");
static_assert(decltype(0000000000000000000000000000000000000000001_13)::value
              == 1u, "");

static_assert(!decltype(1_13)::overflow, "");
static_assert(!decltype(2_13)::overflow, "");
static_assert(!decltype(3_13)::overflow, "");
static_assert(!decltype(4_13)::overflow, "");
static_assert(!decltype(5_13)::overflow, "");
static_assert(!decltype(6_13)::overflow, "");
static_assert(!decltype(7_13)::overflow, "");
static_assert(!decltype(8_13)::overflow, "");
static_assert(!decltype(9_13)::overflow, "");
static_assert(!decltype(10_13)::overflow, "");
static_assert(!decltype(11_13)::overflow, "");
static_assert(!decltype(12_13)::overflow, "");
static_assert(!decltype(13_13)::overflow, "");
static_assert(decltype(14_13)::overflow, "");
static_assert(decltype(1_13)::value == 1u, "");
static_assert(decltype(2_13)::value == 2u, "");
static_assert(decltype(3_13)::value == 3u, "");
static_assert(decltype(4_13)::value == 4u, "");
static_assert(decltype(5_13)::value == 5u, "");
static_assert(decltype(6_13)::value == 6u, "");
static_assert(decltype(7_13)::value == 7u, "");
static_assert(decltype(8_13)::value == 8u, "");
static_assert(decltype(9_13)::value == 9u, "");
static_assert(decltype(10_13)::value == 10u, "");
static_assert(decltype(11_13)::value == 11u, "");
static_assert(decltype(12_13)::value == 12u, "");
static_assert(decltype(13_13)::value == 13u, "");

static_assert(!decltype(0x1_13)::overflow, "");
static_assert(!decltype(0x2_13)::overflow, "");
static_assert(!decltype(0x3_13)::overflow, "");
static_assert(!decltype(0x4_13)::overflow, "");
static_assert(!decltype(0x5_13)::overflow, "");
static_assert(!decltype(0x6_13)::overflow, "");
static_assert(!decltype(0x7_13)::overflow, "");
static_assert(!decltype(0x8_13)::overflow, "");
static_assert(!decltype(0x9_13)::overflow, "");
static_assert(!decltype(0xa_13)::overflow, "");
static_assert(!decltype(0xb_13)::overflow, "");
static_assert(!decltype(0xc_13)::overflow, "");
static_assert(!decltype(0xd_13)::overflow, "");
static_assert(decltype(0xe_13)::overflow, "");
static_assert(decltype(0x1_13)::value == 1u, "");
static_assert(decltype(0x2_13)::value == 2u, "");
static_assert(decltype(0x3_13)::value == 3u, "");
static_assert(decltype(0x4_13)::value == 4u, "");
static_assert(decltype(0x5_13)::value == 5u, "");
static_assert(decltype(0x6_13)::value == 6u, "");
static_assert(decltype(0x7_13)::value == 7u, "");
static_assert(decltype(0x8_13)::value == 8u, "");
static_assert(decltype(0x9_13)::value == 9u, "");
static_assert(decltype(0xa_13)::value == 10u, "");
static_assert(decltype(0xb_13)::value == 11u, "");
static_assert(decltype(0xc_13)::value == 12u, "");
static_assert(decltype(0xd_13)::value == 13u, "");
static_assert(decltype(0x00000000000000000000000000000000000000001_13)::value
              == 1u, "");

static_assert(!decltype(0X1_13)::overflow, "");
static_assert(!decltype(0X2_13)::overflow, "");
static_assert(!decltype(0X3_13)::overflow, "");
static_assert(!decltype(0X4_13)::overflow, "");
static_assert(!decltype(0X5_13)::overflow, "");
static_assert(!decltype(0X6_13)::overflow, "");
static_assert(!decltype(0X7_13)::overflow, "");
static_assert(!decltype(0X8_13)::overflow, "");
static_assert(!decltype(0X9_13)::overflow, "");
static_assert(!decltype(0Xa_13)::overflow, "");
static_assert(!decltype(0Xb_13)::overflow, "");
static_assert(!decltype(0Xc_13)::overflow, "");
static_assert(!decltype(0Xd_13)::overflow, "");
static_assert(decltype(0Xe_13)::overflow, "");
static_assert(decltype(0X1_13)::value == 1u, "");
static_assert(decltype(0X2_13)::value == 2u, "");
static_assert(decltype(0X3_13)::value == 3u, "");
static_assert(decltype(0X4_13)::value == 4u, "");
static_assert(decltype(0X5_13)::value == 5u, "");
static_assert(decltype(0X6_13)::value == 6u, "");
static_assert(decltype(0X7_13)::value == 7u, "");
static_assert(decltype(0X8_13)::value == 8u, "");
static_assert(decltype(0X9_13)::value == 9u, "");
static_assert(decltype(0Xa_13)::value == 10u, "");
static_assert(decltype(0Xb_13)::value == 11u, "");
static_assert(decltype(0Xc_13)::value == 12u, "");
static_assert(decltype(0Xd_13)::value == 13u, "");
static_assert(decltype(0X00000000000000000000000000000000000000001_13)::value
              == 1u, "");

static_assert(!decltype(0.0_13)::valid, "");
static_assert(!decltype(1.0_13)::valid, "");
static_assert(!decltype(90.0_13)::valid, "");
static_assert(!decltype(0000.0_13)::valid, "");
static_assert(!decltype(0001.0_13)::valid, "");
static_assert(!decltype(0009.0_13)::valid, "");

int main() {}
