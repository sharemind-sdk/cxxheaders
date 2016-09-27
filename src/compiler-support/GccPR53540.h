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

#ifndef SHAREMIND_GCCPR53540_H
#define SHAREMIND_GCCPR53540_H

#include <sharemind/compiler-support/GccVersion.h>

/*
  This is a workaround around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53540
  which causes code like

    template <typename T>
    struct A { using type = int; };

    template <typename T>
    void f() {
      using U = A<T>;
      using V = U::type; // no type named 'type' in 'using U = struct A<T>'
    }

    int g() { f<int>(); }

  to fail to compile. The workaround is to use the typedef syntax in f()
  instead:

    typedef A<T> U;

  or use the

*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
#define SHAREMIND_GCCPR53540_WORKAROUND(typeName,...) \
    typedef __VA_ARGS__ typeName
#define SHAREMIND_GCCPR53540 true
#else
#define SHAREMIND_GCCPR53540_WORKAROUND(typeName,...) \
    using typeName = __VA_ARGS__
#define SHAREMIND_GCCPR53540 false
#endif

#endif // SHAREMIND_GCCPR53540_H
