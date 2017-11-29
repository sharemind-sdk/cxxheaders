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

#include "../src/EnumToUnderlying.h"

#include <type_traits>
#include "../src/EnumConstant.h"


SHAREMIND_ENUMCONSTANT(bool, v1, true);
SHAREMIND_ENUMCONSTANT(char, v2, 42);
SHAREMIND_ENUMCONSTANT(int, v3, 42);
SHAREMIND_ENUMCONSTANT(unsigned long, v4, 42u);

using sharemind::enumToUnderlying;

static_assert(std::is_same<bool, decltype(enumToUnderlying(v1))>::value, "");
static_assert(std::is_same<char, decltype(enumToUnderlying(v2))>::value, "");
static_assert(std::is_same<int, decltype(enumToUnderlying(v3))>::value, "");
static_assert(std::is_same<unsigned long,
              decltype(enumToUnderlying(v4))>::value, "");

static_assert(enumToUnderlying(v1) == true, "");
static_assert(enumToUnderlying(v2) == 42, "");
static_assert(enumToUnderlying(v3) == 42, "");
static_assert(enumToUnderlying(v4) == 42u, "");

int main() {}
