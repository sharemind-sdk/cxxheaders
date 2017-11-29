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

#include "../src/CopyVolatile.h"
#include <type_traits>


#define SHAREMIND_COPYCONST_TEST(t,f,r) \
    static_assert( \
            std::is_same<\
                sharemind::CopyVolatile<int t, int f>::type, \
                int r \
            >::value, ""); \
    static_assert( \
            std::is_same<\
                sharemind::CopyVolatile_t<int t, int f>, \
                int r \
            >::value, "")

SHAREMIND_COPYCONST_TEST(        ,        ,        );
SHAREMIND_COPYCONST_TEST(volatile,        ,        );
SHAREMIND_COPYCONST_TEST(        ,volatile,volatile);
SHAREMIND_COPYCONST_TEST(volatile,volatile,volatile);

#define SHAREMIND_ORCONST_TEST(t,f,r) \
    static_assert( \
            std::is_same<\
                sharemind::OrVolatile<int t, int f>::type, \
                int r \
            >::value, ""); \
    static_assert( \
            std::is_same<\
                sharemind::OrVolatile_t<int t, int f>, \
                int r \
            >::value, "")

SHAREMIND_ORCONST_TEST(        ,        ,        );
SHAREMIND_ORCONST_TEST(volatile,        ,volatile);
SHAREMIND_ORCONST_TEST(        ,volatile,volatile);
SHAREMIND_ORCONST_TEST(volatile,volatile,volatile);

int main() {}
