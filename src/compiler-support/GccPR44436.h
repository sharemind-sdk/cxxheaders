/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCPR44436_H
#define SHAREMIND_GCCPR44436_H

#include "GccVersion.h"


/*
  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=44436 whereby associative
  containers before GCC 4.8 were lacking emplace* methods has no good
  workaround at this moment.

  The best solution at this point is to write conditional code (based on the
  macros in this file) which for the affected GCC versions enables different
  code not using the emplace* methods.
*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
#ifndef SHAREMIND_SILENCE_WORKAROUND_WARNINGS
#warning Using PR44436 workaround for g++ older than 4.8, which may degrade \
         runtime performance. Define SHAREMIND_SILENCE_WORKAROUND_WARNINGS to \
         silence this warning.
#endif
#define SHAREMIND_GCCPR44436 true
#define SHAREMIND_GCCPR44436_METHOD insert
#else
#define SHAREMIND_GCCPR44436 false
#define SHAREMIND_GCCPR44436_METHOD emplace
#endif

#endif // SHAREMIND_GCCPR44436_H
