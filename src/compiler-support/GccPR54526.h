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

#ifndef SHAREMIND_GCCPR54526_H
#define SHAREMIND_GCCPR54526_H

#include <sharemind/compiler-support/GccVersion.h>

/*
  This is a workaround around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54526
  which parses <:: as a digraph in code like:

    template <> struct S<::A> {};

  Use this workaround as follows:

    template <> struct S<SHAREMIND_GCCPR54526_WORKAROUND::A> {};
*/

#define SHAREMIND_GCCPR54526_WORKAROUND
#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
#define SHAREMIND_GCCPR54526 true
#else
#define SHAREMIND_GCCPR54526 false
#endif

#endif // SHAREMIND_GCCPR54526_H
