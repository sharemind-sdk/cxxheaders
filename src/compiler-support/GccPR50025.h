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

#ifndef SHAREMIND_GCCPR50025_H
#define SHAREMIND_GCCPR50025_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  This is a workaround around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=50025
  which prevents code like the following from working.

    struct X {
        X(X & a) : ri{a} {}
        X & ri;
    };

  As stated in the bug report, this is actually a bug in the C++11 standard,
  defect report 1288:

    http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1288

  Use this workaround as follows:

    struct X {
        X(X & a) : ri SHAREMIND_GCCPR50025_WORKAROUND(a) {}
        X & ri;
    };
*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40900)
#define SHAREMIND_GCCPR50025 true
#define SHAREMIND_GCCPR50025_WORKAROUND(...) (__VA_ARGS__)
#else
#define SHAREMIND_GCCPR50025 false
#define SHAREMIND_GCCPR50025_WORKAROUND(...) {__VA_ARGS__}
#endif

#endif // SHAREMIND_GCCPR50025_H
