/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCPR55015_H
#define SHAREMIND_GCCPR55015_H

#include "GccVersion.h"


/*
  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55015 which causes "undefined
  reference to" errors for lambdas defined inside inlined functions has no good
  workaround at this moment. The affected GCC versions are 4.7.2 an 4.8.0.

  The best solution at this point is to write conditional code (based on the
  SHAREMIND_GCCPR55015 macro) which for the affected GCC versions enables
  different code not using lambdas inside (implicitly) inlined functions and
  methods.
*/

#if defined(SHAREMIND_GCC_VERSION) \
    && ((SHAREMIND_GCC_VERSION == 40702) || (SHAREMIND_GCC_VERSION == 40800))
#define SHAREMIND_GCCPR55015 1
#else
#define SHAREMIND_GCCPR55015 0
#endif

#endif // SHAREMIND_GCCPR55015_H
