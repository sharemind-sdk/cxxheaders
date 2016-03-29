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

#ifndef SHAREMIND_GCCPR55914_H
#define SHAREMIND_GCCPR55914_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55914 whereby template parameter
  packs are not expanded in lambdas. Currently no good workaround.
*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40900)
#define SHAREMIND_GCCPR55914 true
#else
#define SHAREMIND_GCCPR55914 false
#endif

#endif // SHAREMIND_GCCPR55914_H
