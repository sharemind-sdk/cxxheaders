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

#ifndef SHAREMIND_CSTDINTLITERALS_H
#define SHAREMIND_CSTDINTLITERALS_H

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <cstdint>
#include <sharemind/preprocessor.h>
#include "IntegerLiterals.h"


#define SHAREMIND_CSTDINTLITERALS_SIGNED \
    ((Int8,int8,int8)) \
    ((Int16,int16,int16)) \
    ((Int32,int32,int32)) \
    ((Int64,int64,int64)) \
    ((IntFast8,intFast8,int_fast8)) \
    ((IntFast16,intFast16,int_fast16)) \
    ((IntFast32,intFast32,int_fast32)) \
    ((IntFast64,intFast64,int_fast64)) \
    ((IntLeast8,intLeast8,int_least8)) \
    ((IntLeast16,intLeast16,int_least16)) \
    ((IntLeast32,intLeast32,int_least32)) \
    ((IntLeast64,intLeast64,int_least64)) \
    ((IntMax,intMax,intmax)) \
    ((IntPtr,intPtr,intptr))
#define SHAREMIND_CSTDINTLITERALS_UNSIGNED \
    ((Uint8,uint8,uint8)) \
    ((Uint16,uint16,uint16)) \
    ((Uint32,uint32,uint32)) \
    ((Uint64,uint64,uint64)) \
    ((UintFast8,uintFast8,uint_fast8)) \
    ((UintFast16,uintFast16,uint_fast16)) \
    ((UintFast32,uintFast32,uint_fast32)) \
    ((UintFast64,uintFast64,uint_fast64)) \
    ((UintLeast8,uintLeast8,uint_least8)) \
    ((UintLeast16,uintLeast16,uint_least16)) \
    ((UintLeast32,uintLeast32,uint_least32)) \
    ((UintLeast64,uintLeast64,uint_least64)) \
    ((UintMax,uintMax,uintmax)) \
    ((UintPtr,uintPtr,uintptr))

#define SHAREMIND_CSTDINTLITERALS_LIT(unused,unused2,e) \
    SHAREMIND_INTEGERLITERAL_SIMPLE_TEMPLATE( \
        SHAREMIND_C(SHAREMIND_T(3,0,e),Literal), \
        std::SHAREMIND_C(SHAREMIND_T(3,2,e),_t))

SHAREMIND_INTEGERLITERAL_NS_WRAP(
    BOOST_PP_SEQ_FOR_EACH(SHAREMIND_CSTDINTLITERALS_LIT,_,
                          SHAREMIND_CSTDINTLITERALS_SIGNED)
    BOOST_PP_SEQ_FOR_EACH(SHAREMIND_CSTDINTLITERALS_LIT,_,
                          SHAREMIND_CSTDINTLITERALS_UNSIGNED)
)
#undef SHAREMIND_CSTDINTLITERALS_LIT

namespace sharemind {

#define SHAREMIND_CSTDINTLITERALS_SIGNED_OP(unused,unused2,e) \
    SHAREMIND_INTEGERLITERAL_SIGNED_OPS( \
        std::SHAREMIND_C(SHAREMIND_T(3,2,e),_t), \
        SHAREMIND_T(3,1,e), \
        SHAREMIND_INTEGERLITERAL_NS :: SHAREMIND_C(SHAREMIND_T(3,0,e),Literal))
BOOST_PP_SEQ_FOR_EACH(SHAREMIND_CSTDINTLITERALS_SIGNED_OP,_,
                      SHAREMIND_CSTDINTLITERALS_SIGNED)
#undef SHAREMIND_CSTDINTLITERALS_SIGNED_OP
#define SHAREMIND_CSTDINTLITERALS_UNSIGNED_OP(unused,unused2,e) \
    SHAREMIND_INTEGERLITERAL_UNSIGNED_OPS( \
        std::SHAREMIND_C(SHAREMIND_T(3,2,e),_t), \
        SHAREMIND_T(2,1,e), \
        SHAREMIND_INTEGERLITERAL_NS :: SHAREMIND_C(SHAREMIND_T(3,0,e),Literal))
BOOST_PP_SEQ_FOR_EACH(SHAREMIND_CSTDINTLITERALS_UNSIGNED_OP,_,
                      SHAREMIND_CSTDINTLITERALS_UNSIGNED)
#undef SHAREMIND_CSTDINTLITERALS_UNSIGNED_OP

} /* namespace sharemind { */

#undef SHAREMIND_CSTDINTLITERALS_SIGNED
#undef SHAREMIND_CSTDINTLITERALS_UNSIGNED

#endif /* #ifndef SHAREMIND_CSTDINTLITERALS_H */
