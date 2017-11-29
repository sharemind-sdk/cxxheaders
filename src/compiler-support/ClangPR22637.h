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

#ifndef SHAREMIND_CLANGPR26692_H
#define SHAREMIND_CLANGPR26692_H

#include <sharemind/compiler-support/ClangVersion.h>

/*
  This is a workaround around https://llvm.org/bugs/show_bug.cgi?id=22637
  which prevents code like the following from working.

    void f(int const &);
    void f(int &) = delete;

    template <typename>
    struct X {
        int a;
        union { int b; };

        void g() const {
            f(a); // works
            f(b); // error: call to deleted function 'f'
        }
    };

    void h() { X<int>().g(); }

  Use this workaround as follows:

    template <typename T>
    void X<T>::g() const {
        f(a);
        f(SHAREMIND_CLANGPR22637_WORKAROUND(b));
    }

*/


#if defined(SHAREMIND_CLANG_VERSION)
#include <type_traits>

#define SHAREMIND_CLANGPR22637 1
#define SHAREMIND_CLANGPR22637_WORKAROUND(...) \
        const_cast<typename std::remove_reference<decltype(__VA_ARGS__)>::type \
                   const &>(__VA_ARGS__)
#else
#define SHAREMIND_CLANGPR22637 0
#define SHAREMIND_CLANGPR22637_WORKAROUND(...) __VA_ARGS__
#endif

#endif /* SHAREMIND_CLANGPR26692_H */
