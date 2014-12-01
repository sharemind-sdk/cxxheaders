/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCPR50025_H
#define SHAREMIND_GCCPR50025_H

#include "GccVersion.h"


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
        X(X & a) : ri SHAREMIND_GCCPR50025_WORKAROUND(a} {}
        X & ri;
    };
*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
#define SHAREMIND_GCCPR50025_WORKAROUND(...) (__VA_ARGS__)
#else
#define SHAREMIND_GCCPR50025_WORKAROUND(...) {__VA_ARGS__}
#endif

#endif // SHAREMIND_GCCPR50025_H
