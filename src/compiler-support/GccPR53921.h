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

#ifndef SHAREMIND_GCCPR53921_H
#define SHAREMIND_GCCPR53921_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  This is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53921
  which causes an ICE in GCC 4.7.2 and earlier for code like

    void g(int) {}
    template <class X>
    struct A {
        void f() { [this]() { g(v); };}
        int v;
    };

  Use this workaround as follows:

    void g(int) {}
    template <class X>
    struct A {
        void f() { [this]() { g(SHAREMIND_GCCPR53921_WORKAROUND v); };}
        int v;
    };

*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40703)
#define SHAREMIND_GCCPR53921 true
#define SHAREMIND_GCCPR53921_WORKAROUND this->
#else
#define SHAREMIND_GCCPR53921 false
#define SHAREMIND_GCCPR53921_WORKAROUND
#endif

#endif // SHAREMIND_GCCPR53921_H
