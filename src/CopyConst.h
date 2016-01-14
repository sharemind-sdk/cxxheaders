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

#ifndef SHAREMIND_COPYCONST_H
#define SHAREMIND_COPYCONST_H

#include <type_traits>


namespace sharemind {
namespace Detail {

template <typename To, typename From, bool ClearFirst = false>
using CopyConst_ =
        std::conditional<
            std::is_const<From>::value,
            typename std::add_const<To>::type,
            typename std::conditional<
                ClearFirst,
                typename std::remove_const<To>::type,
                To
            >::type
        >;

#define SHAREMIND_COPYCONST_TEST(t,f,c,r) \
    static_assert( \
            std::is_same<CopyConst_<int t, int f, c>::type, int r>::value, \
            "")
SHAREMIND_COPYCONST_TEST(     ,     ,false,     );
SHAREMIND_COPYCONST_TEST(     ,     ,true ,     );
SHAREMIND_COPYCONST_TEST(const,     ,false,const);
SHAREMIND_COPYCONST_TEST(const,     ,true ,     );
SHAREMIND_COPYCONST_TEST(     ,const,false,const);
SHAREMIND_COPYCONST_TEST(     ,const,true ,const);
SHAREMIND_COPYCONST_TEST(const,const,false,const);
SHAREMIND_COPYCONST_TEST(const,const,true ,const);
#undef SHAREMIND_COPYCONST_TEST

} /* namespace Detail { */

template <typename To, typename From>
using CopyConst = Detail::CopyConst_<To, From, true>;

template <typename To, typename From>
using CopyConst_t = typename CopyConst<To, From>::type;

template <typename To, typename From>
using OrConst = Detail::CopyConst_<To, From, false>;

template <typename To, typename From>
using OrConst_t = typename OrConst<To, From>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_COPYCONST_H */
