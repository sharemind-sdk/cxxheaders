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

#ifndef SHAREMIND_GCCPR53137_H
#define SHAREMIND_GCCPR53137_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  This is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53137
  according to which GCC 4.7 before 4.7.3 and GCC 4.8 before 4.8.1 incorrectly
  handle lambda-captured this pointer, resulting in an ICE.

  Note that this is a similar issue to PR54277, but not identical. The
  workaround is the same though.

  To work around this issue, prepend SHAREMIND_GCCPR53137_WORKAROUND to the
  member name, e.g.:

      [this] { SHAREMIND_GCCPR53137_WORKAROUND m_myMember(); }
*/

#if defined(SHAREMIND_GCC_VERSION) \
    && ((SHAREMIND_GCC_VERSION < 40703) \
        || ((SHAREMIND_GCC_VERSION >= 40800) \
            && (SHAREMIND_GCC_VERSION < 40801)))
#define SHAREMIND_GCCPR53137 true
#define SHAREMIND_GCCPR53137_WORKAROUND this->
#else
#define SHAREMIND_GCCPR53137 false
#define SHAREMIND_GCCPR53137_WORKAROUND
#endif

#endif // SHAREMIND_GCCPR53137_H
