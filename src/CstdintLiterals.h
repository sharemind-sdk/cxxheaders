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

#include <cstdint>
#include "IntegerLiterals.h"


namespace sharemind {

SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int8)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int16)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int32)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int64)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_fast8)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_fast16)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_fast32)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_fast64)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_least8)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_least16)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_least32)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(int_least64)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(intmax)
SHAREMIND_INTEGERLITERAL_DEFINE_SIGNED(intptr)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint8)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint16)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint32)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint64)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_fast8)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_fast16)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_fast32)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_fast64)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_least8)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_least16)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_least32)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uint_least64)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uintmax)
SHAREMIND_INTEGERLITERAL_DEFINE_UNSIGNED(uintptr)

} /* namespace sharemind { */

#endif /* #ifndef SHAREMIND_CSTDINTLITERALS_H */
