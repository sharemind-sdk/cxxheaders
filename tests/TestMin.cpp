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

#include "../src/Min.h"

#include "../src/EnumConstant.h"


SHAREMIND_ENUMCONSTANT(int, v1, 1);
SHAREMIND_ENUMCONSTANT(int, v2, 2);
SHAREMIND_ENUMCONSTANT(int, v3, 3);

int main() {
    using sharemind::min;
    static_assert(min(1) == 1, "");
    static_assert(min(1, 2) == 1, "");
    static_assert(min(2, 1) == 1, "");
    static_assert(min(1, 2, 3) == 1, "");
    static_assert(min(1, 3, 2) == 1, "");
    static_assert(min(2, 1, 3) == 1, "");
    static_assert(min(2, 3, 1) == 1, "");
    static_assert(min(3, 1, 2) == 1, "");
    static_assert(min(3, 2, 1) == 1, "");

    static_assert(min(v1) == 1, "");
    static_assert(min(v1, 2) == 1, "");
    static_assert(min(2, v1) == 1, "");
    static_assert(min(v1, 2, 3) == 1, "");
    static_assert(min(v1, 3, 2) == 1, "");
    static_assert(min(2, v1, 3) == 1, "");
    static_assert(min(2, 3, v1) == 1, "");
    static_assert(min(3, v1, 2) == 1, "");
    static_assert(min(3, 2, v1) == 1, "");

    static_assert(min(v1) == 1, "");
    static_assert(min(v1, v2) == 1, "");
    static_assert(min(v2, v1) == 1, "");
    static_assert(min(v1, v2, v3) == 1, "");
    static_assert(min(v1, v3, v2) == 1, "");
    static_assert(min(v2, v1, v3) == 1, "");
    static_assert(min(v2, v3, v1) == 1, "");
    static_assert(min(v3, v1, v2) == 1, "");
    static_assert(min(v3, v2, v1) == 1, "");
}
