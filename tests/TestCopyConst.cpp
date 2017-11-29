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

#include "../src/CopyConst.h"
#include <type_traits>


#define SHAREMIND_COPYCONST_TEST(t,f,r) \
    static_assert( \
            std::is_same<\
                sharemind::CopyConst<int t, int f>::type, \
                int r \
            >::value, ""); \
    static_assert( \
            std::is_same<\
                sharemind::CopyConst_t<int t, int f>, \
                int r \
            >::value, "")

SHAREMIND_COPYCONST_TEST(     ,     ,     );
SHAREMIND_COPYCONST_TEST(const,     ,     );
SHAREMIND_COPYCONST_TEST(     ,const,const);
SHAREMIND_COPYCONST_TEST(const,const,const);

#define SHAREMIND_ORCONST_TEST(t,f,r) \
    static_assert( \
            std::is_same<\
                sharemind::OrConst<int t, int f>::type, \
                int r \
            >::value, ""); \
    static_assert( \
            std::is_same<\
                sharemind::OrConst_t<int t, int f>, \
                int r \
            >::value, "")

SHAREMIND_ORCONST_TEST(     ,     ,     );
SHAREMIND_ORCONST_TEST(const,     ,const);
SHAREMIND_ORCONST_TEST(     ,const,const);
SHAREMIND_ORCONST_TEST(const,const,const);

int main() {}
