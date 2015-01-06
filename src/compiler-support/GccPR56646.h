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
