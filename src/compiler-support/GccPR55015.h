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

#ifndef SHAREMIND_GCCPR55015_H
#define SHAREMIND_GCCPR55015_H

#include <sharemind/compiler-support/GccVersion.h>


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
#define SHAREMIND_GCCPR55015 true
#else
#define SHAREMIND_GCCPR55015 false
#endif

#endif // SHAREMIND_GCCPR55015_H
