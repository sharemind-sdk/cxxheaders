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

#include "../src/TemplatePrependTypes.h"

#include <type_traits>


using namespace sharemind;

template <typename ...> struct Tmpl;
struct T1; struct T2; struct T3; struct T4;

#define TEST_PREPEND_NONE(...) \
    static_assert( \
        std::is_same< \
            TemplatePrependTypes_t<Tmpl<__VA_ARGS__> >, \
            Tmpl<__VA_ARGS__> \
        >::value, "");
TEST_PREPEND_NONE()
TEST_PREPEND_NONE(T1)
TEST_PREPEND_NONE(T1, T2)
TEST_PREPEND_NONE(T1, T2, T3)
TEST_PREPEND_NONE(T1, T2, T3, T4)

#define TEST_EMPTY(...) \
    static_assert( \
        std::is_same< \
            TemplatePrependTypes_t<Tmpl<>, __VA_ARGS__>, \
            Tmpl<__VA_ARGS__> \
        >::value, "");
TEST_EMPTY(T1)
TEST_EMPTY(T1, T2)
TEST_EMPTY(T1, T2, T3)
TEST_EMPTY(T1, T2, T3, T4)

#define WRAP(...) __VA_ARGS__
#define TEST(orig,...) \
    static_assert( \
        std::is_same< \
            TemplatePrependTypes_t<Tmpl<orig>, __VA_ARGS__>, \
            Tmpl<__VA_ARGS__, orig> \
        >::value, "");

TEST(T1, T1)
TEST(T1, T1, T2)
TEST(T1, T1, T2, T3)
TEST(T1, T1, T2, T3, T4)
TEST(WRAP(T1, T2), T1)
TEST(WRAP(T1, T2), T1, T2)
TEST(WRAP(T1, T2), T1, T2, T3)
TEST(WRAP(T1, T2), T1, T2, T3, T4)
TEST(WRAP(T1, T2, T3), T1)
TEST(WRAP(T1, T2, T3), T1, T2)
TEST(WRAP(T1, T2, T3), T1, T2, T3)
TEST(WRAP(T1, T2, T3), T1, T2, T3, T4)
TEST(WRAP(T1, T2, T3, T4), T1)
TEST(WRAP(T1, T2, T3, T4), T1, T2)
TEST(WRAP(T1, T2, T3, T4), T1, T2, T3)
TEST(WRAP(T1, T2, T3, T4), T1, T2, T3, T4)

int main() {}
