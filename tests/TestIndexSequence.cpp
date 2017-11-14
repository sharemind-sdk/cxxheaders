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

#include "../src/IndexSequence.h"

#include <type_traits>


using sharemind::IndexSequence;
using sharemind::MakeIndexSequence;

using T0 = IndexSequence<>;
using T1 = IndexSequence<0u>;
using T2 = IndexSequence<0u, 1u>;
using T3 = IndexSequence<0u, 1u, 2u>;
using T4 = IndexSequence<0u, 1u, 2u, 3u>;
using T5 = IndexSequence<0u, 1u, 2u, 3u, 4u>;
using T6 = IndexSequence<0u, 1u, 2u, 3u, 4u, 5u>;
using T7 = IndexSequence<0u, 1u, 2u, 3u, 4u, 5u, 6u>;
using T8 = IndexSequence<0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u>;
using T9 = IndexSequence<0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u>;
using T10 = IndexSequence<0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u>;
using T11 = IndexSequence<0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u>;
using T12 = IndexSequence<0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u>;

#define TEST(n) \
    static_assert(std::is_same<MakeIndexSequence<n ## u>, T ## n>::value, "");
TEST(0) TEST(1) TEST(2) TEST(3) TEST(4) TEST(5) TEST(6)
TEST(7) TEST(8) TEST(9) TEST(10) TEST(11) TEST(12)

int main() {}
