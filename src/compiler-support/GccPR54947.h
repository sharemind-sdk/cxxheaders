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

#ifndef SHAREMIND_GCCPR54947_H
#define SHAREMIND_GCCPR54947_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  This is a workaround around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54947
  which prevents lambdas with capture inside braced initializer lists.

  To work around this issue, use SHAREMIND_GCCPR54947_WORKAROUND() instead of
  braced initializer lists {}.
*/

#if defined(SHAREMIND_GCC_VERSION) \
    && ((SHAREMIND_GCC_VERSION < 40703) \
        || ((SHAREMIND_GCC_VERSION >= 40800) \
            && (SHAREMIND_GCC_VERSION < 40801)))
#define SHAREMIND_GCCPR54947 true
#define SHAREMIND_GCCPR54947_WORKAROUND(...) (__VA_ARGS__)
#else
#define SHAREMIND_GCCPR54947 false
#define SHAREMIND_GCCPR54947_WORKAROUND(...) {__VA_ARGS__}
#endif

#endif // SHAREMIND_GCCPR54947_H
