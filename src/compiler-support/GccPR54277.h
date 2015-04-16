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

#ifndef SHAREMIND_GCCPR54277_H
#define SHAREMIND_GCCPR54277_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  This is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54277
  according to which GCC before 4.7.3 incorrectly handles implicit uses of a
  lambda-captured this pointer as a pointer to a const object instance.

  To work around this issue, prepend SHAREMIND_GCCPR54277_WORKAROUND to the
  member name, e.g.:

      [this] { SHAREMIND_GCCPR54277_WORKAROUND m_myMember(); }
*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40703)
#define SHAREMIND_GCCPR54277_WORKAROUND this->
#else
#define SHAREMIND_GCCPR54277_WORKAROUND
#endif

#endif // SHAREMIND_GCCPR54277_H
