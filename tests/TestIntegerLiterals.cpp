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
constexpr L13<Cs...> operator "" _dec13() { return L13<Cs...>(); }

static_assert(!decltype(.0_dec13)::valid, "");
static_assert(!decltype(0._dec13)::valid, "");
static_assert(!decltype(0.0_dec13)::valid, "");
static_assert(!decltype(.000_dec13)::valid, "");
static_assert(!decltype(000._dec13)::valid, "");
static_assert(!decltype(0.000_dec13)::valid, "");
static_assert(!decltype(000.0_dec13)::valid, "");
static_assert(!decltype(00000.000000_dec13)::valid, "");
static_assert(!decltype(.0001_dec13)::valid, "");
static_assert(!decltype(0001._dec13)::valid, "");
static_assert(!decltype(.1000_dec13)::valid, "");
static_assert(!decltype(1000._dec13)::valid, "");
static_assert(!decltype(0000E0000_dec13)::valid, "");
static_assert(!decltype(0000e0000_dec13)::valid, "");
static_assert(!decltype(1234E0000_dec13)::valid, "");
static_assert(!decltype(1234e0000_dec13)::valid, "");
static_assert(!decltype(0000E-0000_dec13)::valid, "");
static_assert(!decltype(0000e-0000_dec13)::valid, "");
static_assert(!decltype(1234E-0000_dec13)::valid, "");
static_assert(!decltype(1234e-0000_dec13)::valid, "");
static_assert(!decltype(0000E+0000_dec13)::valid, "");
static_assert(!decltype(0000e+0000_dec13)::valid, "");
static_assert(!decltype(1234E+0000_dec13)::valid, "");
static_assert(!decltype(1234e+0000_dec13)::valid, "");
static_assert(!decltype(0000E+0000_dec13)::valid, "");
static_assert(!decltype(0000e+0000_dec13)::valid, "");
static_assert(!decltype(1234E+0000_dec13)::valid, "");
static_assert(!decltype(1234e+0000_dec13)::valid, "");

static_assert(decltype(0_dec13)::valid, "");
static_assert(decltype(1234567890_dec13)::valid, "");
static_assert(decltype(01234567_dec13)::valid, "");
static_assert(decltype(0001234567_dec13)::valid, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_dec13)::valid, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_dec13)::valid, "");
static_assert(decltype(0x00001_dec13)::valid, "");
static_assert(decltype(0X00001_dec13)::valid, "");

static_assert(decltype(0_dec13)::base == 8u, "");
static_assert(decltype(1234567890_dec13)::base == 10u, "");
static_assert(decltype(01234567_dec13)::base == 8u, "");
static_assert(decltype(0001234567_dec13)::base == 8u, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_dec13)::base == 16u, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_dec13)::base == 16u, "");
static_assert(decltype(0x00001_dec13)::base == 16u, "");
static_assert(decltype(0X00001_dec13)::base == 16u, "");

static_assert(decltype(0_dec13)::digits == 0u, "");
static_assert(decltype(1234567890_dec13)::digits == 10u, "");
static_assert(decltype(01234567_dec13)::digits == 7u, "");
static_assert(decltype(0001234567_dec13)::digits == 9u, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_dec13)::digits == 22u, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_dec13)::digits == 22u, "");
static_assert(decltype(0x00001_dec13)::digits == 5u, "");
static_assert(decltype(0X00001_dec13)::digits == 5u, "");

static_assert(decltype(0_dec13)::chars == 1u, "");
static_assert(decltype(1234567890_dec13)::chars == 10u, "");
static_assert(decltype(01234567_dec13)::chars == 8u, "");
static_assert(decltype(0001234567_dec13)::chars == 10u, "");
static_assert(decltype(0x123456789abcdef0ABCDEF_dec13)::chars == 24u, "");
static_assert(decltype(0X123456789abcdef0ABCDEF_dec13)::chars == 24u, "");
static_assert(decltype(0x00001_dec13)::chars == 7u, "");
static_assert(decltype(0X00001_dec13)::chars == 7u, "");

static_assert(!decltype(0_dec13)::overflow, "");
static_assert(!decltype(01_dec13)::overflow, "");
static_assert(!decltype(02_dec13)::overflow, "");
static_assert(!decltype(03_dec13)::overflow, "");
static_assert(!decltype(04_dec13)::overflow, "");
static_assert(!decltype(05_dec13)::overflow, "");
static_assert(!decltype(06_dec13)::overflow, "");
static_assert(!decltype(07_dec13)::overflow, "");
static_assert(!decltype(010_dec13)::overflow, "");
static_assert(!decltype(011_dec13)::overflow, "");
static_assert(!decltype(012_dec13)::overflow, "");
static_assert(!decltype(013_dec13)::overflow, "");
static_assert(!decltype(014_dec13)::overflow, "");
static_assert(!decltype(015_dec13)::overflow, "");
static_assert(decltype(016_dec13)::overflow, "");
static_assert(decltype(0_dec13)::value == 0u, "");
static_assert(decltype(01_dec13)::value == 1u, "");
static_assert(decltype(02_dec13)::value == 2u, "");
static_assert(decltype(03_dec13)::value == 3u, "");
static_assert(decltype(04_dec13)::value == 4u, "");
static_assert(decltype(05_dec13)::value == 5u, "");
static_assert(decltype(06_dec13)::value == 6u, "");
static_assert(decltype(07_dec13)::value == 7u, "");
static_assert(decltype(010_dec13)::value == 8u, "");
static_assert(decltype(011_dec13)::value == 9u, "");
static_assert(decltype(012_dec13)::value == 10u, "");
static_assert(decltype(013_dec13)::value == 11u, "");
static_assert(decltype(014_dec13)::value == 12u, "");
static_assert(decltype(015_dec13)::value == 13u, "");
static_assert(!decltype(00_dec13)::overflow, "");
static_assert(!decltype(001_dec13)::overflow, "");
static_assert(!decltype(002_dec13)::overflow, "");
static_assert(!decltype(003_dec13)::overflow, "");
static_assert(!decltype(004_dec13)::overflow, "");
static_assert(!decltype(005_dec13)::overflow, "");
static_assert(!decltype(006_dec13)::overflow, "");
static_assert(!decltype(007_dec13)::overflow, "");
static_assert(!decltype(0010_dec13)::overflow, "");
static_assert(!decltype(0011_dec13)::overflow, "");
static_assert(!decltype(0012_dec13)::overflow, "");
static_assert(!decltype(0013_dec13)::overflow, "");
static_assert(!decltype(0014_dec13)::overflow, "");
static_assert(!decltype(0015_dec13)::overflow, "");
static_assert(decltype(0016_dec13)::overflow, "");
static_assert(decltype(00_dec13)::value == 0u, "");
static_assert(decltype(001_dec13)::value == 1u, "");
static_assert(decltype(002_dec13)::value == 2u, "");
static_assert(decltype(003_dec13)::value == 3u, "");
static_assert(decltype(004_dec13)::value == 4u, "");
static_assert(decltype(005_dec13)::value == 5u, "");
static_assert(decltype(006_dec13)::value == 6u, "");
static_assert(decltype(007_dec13)::value == 7u, "");
static_assert(decltype(0010_dec13)::value == 8u, "");
static_assert(decltype(0011_dec13)::value == 9u, "");
static_assert(decltype(0012_dec13)::value == 10u, "");
static_assert(decltype(0013_dec13)::value == 11u, "");
static_assert(decltype(0014_dec13)::value == 12u, "");
static_assert(decltype(0015_dec13)::value == 13u, "");
static_assert(decltype(0000000000000000000000000000000000000000001_dec13)::value
              == 1u, "");

static_assert(!decltype(1_dec13)::overflow, "");
static_assert(!decltype(2_dec13)::overflow, "");
static_assert(!decltype(3_dec13)::overflow, "");
static_assert(!decltype(4_dec13)::overflow, "");
static_assert(!decltype(5_dec13)::overflow, "");
static_assert(!decltype(6_dec13)::overflow, "");
static_assert(!decltype(7_dec13)::overflow, "");
static_assert(!decltype(8_dec13)::overflow, "");
static_assert(!decltype(9_dec13)::overflow, "");
static_assert(!decltype(10_dec13)::overflow, "");
static_assert(!decltype(11_dec13)::overflow, "");
static_assert(!decltype(12_dec13)::overflow, "");
static_assert(!decltype(13_dec13)::overflow, "");
static_assert(decltype(14_dec13)::overflow, "");
static_assert(decltype(1_dec13)::value == 1u, "");
static_assert(decltype(2_dec13)::value == 2u, "");
static_assert(decltype(3_dec13)::value == 3u, "");
static_assert(decltype(4_dec13)::value == 4u, "");
static_assert(decltype(5_dec13)::value == 5u, "");
static_assert(decltype(6_dec13)::value == 6u, "");
static_assert(decltype(7_dec13)::value == 7u, "");
static_assert(decltype(8_dec13)::value == 8u, "");
static_assert(decltype(9_dec13)::value == 9u, "");
static_assert(decltype(10_dec13)::value == 10u, "");
static_assert(decltype(11_dec13)::value == 11u, "");
static_assert(decltype(12_dec13)::value == 12u, "");
static_assert(decltype(13_dec13)::value == 13u, "");

static_assert(!decltype(0x1_dec13)::overflow, "");
static_assert(!decltype(0x2_dec13)::overflow, "");
static_assert(!decltype(0x3_dec13)::overflow, "");
static_assert(!decltype(0x4_dec13)::overflow, "");
static_assert(!decltype(0x5_dec13)::overflow, "");
static_assert(!decltype(0x6_dec13)::overflow, "");
static_assert(!decltype(0x7_dec13)::overflow, "");
static_assert(!decltype(0x8_dec13)::overflow, "");
static_assert(!decltype(0x9_dec13)::overflow, "");
static_assert(!decltype(0xa_dec13)::overflow, "");
static_assert(!decltype(0xb_dec13)::overflow, "");
static_assert(!decltype(0xc_dec13)::overflow, "");
static_assert(!decltype(0xd_dec13)::overflow, "");
static_assert(decltype(0xe_dec13)::overflow, "");
static_assert(decltype(0x1_dec13)::value == 1u, "");
static_assert(decltype(0x2_dec13)::value == 2u, "");
static_assert(decltype(0x3_dec13)::value == 3u, "");
static_assert(decltype(0x4_dec13)::value == 4u, "");
static_assert(decltype(0x5_dec13)::value == 5u, "");
static_assert(decltype(0x6_dec13)::value == 6u, "");
static_assert(decltype(0x7_dec13)::value == 7u, "");
static_assert(decltype(0x8_dec13)::value == 8u, "");
static_assert(decltype(0x9_dec13)::value == 9u, "");
static_assert(decltype(0xa_dec13)::value == 10u, "");
static_assert(decltype(0xb_dec13)::value == 11u, "");
static_assert(decltype(0xc_dec13)::value == 12u, "");
static_assert(decltype(0xd_dec13)::value == 13u, "");
static_assert(decltype(0x00000000000000000000000000000000000000001_dec13)::value
              == 1u, "");

static_assert(!decltype(0X1_dec13)::overflow, "");
static_assert(!decltype(0X2_dec13)::overflow, "");
static_assert(!decltype(0X3_dec13)::overflow, "");
static_assert(!decltype(0X4_dec13)::overflow, "");
static_assert(!decltype(0X5_dec13)::overflow, "");
static_assert(!decltype(0X6_dec13)::overflow, "");
static_assert(!decltype(0X7_dec13)::overflow, "");
static_assert(!decltype(0X8_dec13)::overflow, "");
static_assert(!decltype(0X9_dec13)::overflow, "");
static_assert(!decltype(0Xa_dec13)::overflow, "");
static_assert(!decltype(0Xb_dec13)::overflow, "");
static_assert(!decltype(0Xc_dec13)::overflow, "");
static_assert(!decltype(0Xd_dec13)::overflow, "");
static_assert(decltype(0Xe_dec13)::overflow, "");
static_assert(decltype(0X1_dec13)::value == 1u, "");
static_assert(decltype(0X2_dec13)::value == 2u, "");
static_assert(decltype(0X3_dec13)::value == 3u, "");
static_assert(decltype(0X4_dec13)::value == 4u, "");
static_assert(decltype(0X5_dec13)::value == 5u, "");
static_assert(decltype(0X6_dec13)::value == 6u, "");
static_assert(decltype(0X7_dec13)::value == 7u, "");
static_assert(decltype(0X8_dec13)::value == 8u, "");
static_assert(decltype(0X9_dec13)::value == 9u, "");
static_assert(decltype(0Xa_dec13)::value == 10u, "");
static_assert(decltype(0Xb_dec13)::value == 11u, "");
static_assert(decltype(0Xc_dec13)::value == 12u, "");
static_assert(decltype(0Xd_dec13)::value == 13u, "");
static_assert(decltype(0X00000000000000000000000000000000000000001_dec13)::value
              == 1u, "");

static_assert(!decltype(0.0_dec13)::valid, "");
static_assert(!decltype(1.0_dec13)::valid, "");
static_assert(!decltype(90.0_dec13)::valid, "");
static_assert(!decltype(0000.0_dec13)::valid, "");
static_assert(!decltype(0001.0_dec13)::valid, "");
static_assert(!decltype(0009.0_dec13)::valid, "");

int main() {}
