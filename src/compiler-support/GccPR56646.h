/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCPR56646_H
#define SHAREMIND_GCCPR56646_H

#include "GccVersion.h"


/*
  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56646 which causes an internal
  compiler error (ICE) when structs with functions are used inside methods of
  another class. The affected GCC versions are 4.7.2 an 4.8.0, and possibly also
  versions 4.7.0 and 4.7.1.

  The best solution at this point is to write conditional code (based on the
  SHAREMIND_GCCPR56646 macro) which for the affected GCC versions enables
  different code not using such structs inside other methods.
*/

#if defined(SHAREMIND_GCC_VERSION) \
    && (((SHAREMIND_GCC_VERSION >= 40700) && (SHAREMIND_GCC_VERSION < 40703)) \
        || (SHAREMIND_GCC_VERSION == 40800))
#define SHAREMIND_GCCPR56646 true
#else
#define SHAREMIND_GCCPR56646 false
#endif

#endif // SHAREMIND_GCCPR56646_H
