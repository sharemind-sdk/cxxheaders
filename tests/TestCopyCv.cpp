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

#include "../src/CopyCv.h"
#include <type_traits>


#define SHAREMIND_COPYCV_TEST(t,f,r) \
    static_assert( \
            std::is_same<\
                sharemind::CopyCv<int t, int f>::type, \
                int r \
            >::value, ""); \
    static_assert( \
            std::is_same<\
                sharemind::CopyCv_t<int t, int f>, \
                int r \
            >::value, "")

#define C const
#define V volatile
#define CV const volatile

SHAREMIND_COPYCV_TEST(  ,  ,  );
SHAREMIND_COPYCV_TEST(C ,  ,  );
SHAREMIND_COPYCV_TEST(V ,  ,  );
SHAREMIND_COPYCV_TEST(CV,  ,  );
SHAREMIND_COPYCV_TEST(  ,C ,C );
SHAREMIND_COPYCV_TEST(C ,C ,C );
SHAREMIND_COPYCV_TEST(V ,C ,C );
SHAREMIND_COPYCV_TEST(CV,C ,C );
SHAREMIND_COPYCV_TEST(  ,V ,V );
SHAREMIND_COPYCV_TEST(C ,V ,V );
SHAREMIND_COPYCV_TEST(V ,V ,V );
SHAREMIND_COPYCV_TEST(CV,V ,V );
SHAREMIND_COPYCV_TEST(  ,CV,CV);
SHAREMIND_COPYCV_TEST(C ,CV,CV);
SHAREMIND_COPYCV_TEST(V ,CV,CV);
SHAREMIND_COPYCV_TEST(CV,CV,CV);

#define SHAREMIND_ORCV_TEST(t,f,r) \
    static_assert( \
            std::is_same<\
                sharemind::OrCv<int t, int f>::type, \
                int r \
            >::value, ""); \
    static_assert( \
            std::is_same<\
                sharemind::OrCv_t<int t, int f>, \
                int r \
            >::value, "")

SHAREMIND_ORCV_TEST(  ,  ,  );
SHAREMIND_ORCV_TEST(C ,  ,C );
SHAREMIND_ORCV_TEST(V ,  ,V );
SHAREMIND_ORCV_TEST(CV,  ,CV);
SHAREMIND_ORCV_TEST(  ,C ,C );
SHAREMIND_ORCV_TEST(C ,C ,C );
SHAREMIND_ORCV_TEST(V ,C ,CV);
SHAREMIND_ORCV_TEST(CV,C ,CV);
SHAREMIND_ORCV_TEST(  ,V ,V );
SHAREMIND_ORCV_TEST(C ,V ,CV);
SHAREMIND_ORCV_TEST(V ,V ,V );
SHAREMIND_ORCV_TEST(CV,V ,CV);
SHAREMIND_ORCV_TEST(  ,CV,CV);
SHAREMIND_ORCV_TEST(C ,CV,CV);
SHAREMIND_ORCV_TEST(V ,CV,CV);
SHAREMIND_ORCV_TEST(CV,CV,CV);

int main() {}
