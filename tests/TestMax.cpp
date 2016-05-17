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

#include "../src/Max.h"


int main() {
    using sharemind::max;
    static_assert(max(1) == 1, "");
    static_assert(max(1, 2) == 2, "");
    static_assert(max(2, 1) == 2, "");
    static_assert(max(1, 2, 3) == 3, "");
    static_assert(max(1, 3, 2) == 3, "");
    static_assert(max(2, 1, 3) == 3, "");
    static_assert(max(2, 3, 1) == 3, "");
    static_assert(max(3, 1, 2) == 3, "");
    static_assert(max(3, 2, 1) == 3, "");
}
