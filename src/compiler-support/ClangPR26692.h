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

#ifndef SHAREMIND_CLANGPR26692_H
#define SHAREMIND_CLANGPR26692_H

#include <sharemind/compiler-support/ClangVersion.h>


/*
  This is a workaround around https://llvm.org/bugs/show_bug.cgi?id=26692
  which prevents code like the following from working.

    namespace SomeNamespace {
    template <template <typename> class> using T = void;
    template <typename> struct A { using T2 = T<A>; };
    }

  Use this workaround as follows:

    namespace SomeNamespace {
    template <template <typename> class> using T = void;
    template <typename> struct A {
        using T2 = T<SHAREMIND_CLANGPR26692_WORKAROUND(SomeNamespace) A>;
    };
    }

*/

#if defined(SHAREMIND_CLANG_VERSION)
#define SHAREMIND_CLANGPR26692 true
#define SHAREMIND_CLANGPR26692_WORKAROUND(...) __VA_ARGS__ ::
#else
#define SHAREMIND_CLANGPR26692 false
#define SHAREMIND_CLANGPR26692_WORKAROUND(...)
#endif

#endif /* SHAREMIND_CLANGPR26692_H */
