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

#include "../src/TemplateAppend.h"

#include <type_traits>


using namespace sharemind;

template <int ...> struct Tmpl;

#define TEST_PREPEND_NONE(...) \
    static_assert( \
        std::is_same< \
            TemplateAppend_t<int, Tmpl<__VA_ARGS__> >, \
            Tmpl<__VA_ARGS__> \
        >::value, "");
TEST_PREPEND_NONE()
TEST_PREPEND_NONE(1)
TEST_PREPEND_NONE(1, 2)
TEST_PREPEND_NONE(1, 2, 3)
TEST_PREPEND_NONE(1, 2, 3, 4)

#define TEST_EMPTY(...) \
    static_assert( \
        std::is_same< \
            TemplateAppend_t<int, Tmpl<>, __VA_ARGS__>, \
            Tmpl<__VA_ARGS__> \
        >::value, "");
TEST_EMPTY(1)
TEST_EMPTY(1, 2)
TEST_EMPTY(1, 2, 3)
TEST_EMPTY(1, 2, 3, 4)

#define WRAP(...) __VA_ARGS__
#define TEST(orig,...) \
    static_assert( \
        std::is_same< \
            TemplateAppend_t<int, Tmpl<orig>, __VA_ARGS__>, \
            Tmpl<orig, __VA_ARGS__> \
        >::value, "");

TEST(1, 1)
TEST(1, 1, 2)
TEST(1, 1, 2, 3)
TEST(1, 1, 2, 3, 4)
TEST(WRAP(1, 2), 1)
TEST(WRAP(1, 2), 1, 2)
TEST(WRAP(1, 2), 1, 2, 3)
TEST(WRAP(1, 2), 1, 2, 3, 4)
TEST(WRAP(1, 2, 3), 1)
TEST(WRAP(1, 2, 3), 1, 2)
TEST(WRAP(1, 2, 3), 1, 2, 3)
TEST(WRAP(1, 2, 3), 1, 2, 3, 4)
TEST(WRAP(1, 2, 3, 4), 1)
TEST(WRAP(1, 2, 3, 4), 1, 2)
TEST(WRAP(1, 2, 3, 4), 1, 2, 3)
TEST(WRAP(1, 2, 3, 4), 1, 2, 3, 4)

int main() {}
