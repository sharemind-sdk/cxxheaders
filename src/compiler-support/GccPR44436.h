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

#ifndef SHAREMIND_GCCPR44436_H
#define SHAREMIND_GCCPR44436_H

#include <sharemind/compiler-support/GccVersion.h>


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
