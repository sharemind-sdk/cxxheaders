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

#ifndef SHAREMIND_CLANGPR23383_H
#define SHAREMIND_CLANGPR23383_H

#include <sharemind/compiler-support/ClangVersion.h>

/*
  This is a workaround around https://llvm.org/bugs/show_bug.cgi?id=23383
  which prevents code like the following from working.

    template <typename>
    struct Check { constexpr static bool value = true; };

    struct A {
        A() noexcept(Check<int>::value) = default;
    };

  Use this workaround as follows:

    struct A {
        A() noexcept(
                SHAREMIND_CLANGPR23383_WORKAROUND(check1, Check<int>::value))
                = default;

        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF(
                check1,
                Check<int>::value)
    };

*/

/** \todo This might be already fixed in development versions of Clang. As of
          January 19, 2018 this no longer reproduces in with Clang trunk in
          Compiler Explorer (gcc.godbolt.org). The following guard must be
          updated accordingly once a new version of Clang which fixes this issue
          is released (e.g. version 6.0.0). */
#if defined(SHAREMIND_CLANG_VERSION) && ( \
    (SHAREMIND_CLANG_VERSION < 30400) \
    || (SHAREMIND_CLANG_VERSION >= 30600))
#define SHAREMIND_CLANGPR23383 1
#define SHAREMIND_CLANGPR23383_WORKAROUND(tmpName,...) \
    sharemindClangPR23383_ ## tmpName
#define SHAREMIND_CLANGPR23383_WORKAROUND_CONSTEXPR_DEF(tmpName,...) \
    constexpr auto const sharemindClangPR23383_ ## tmpName = __VA_ARGS__;
#define SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF(t,...) \
    private: \
    static SHAREMIND_CLANGPR23383_WORKAROUND_CONSTEXPR_DEF(t, __VA_ARGS__)

#else
#define SHAREMIND_CLANGPR23383 0
#define SHAREMIND_CLANGPR23383_WORKAROUND(tmpName,...) __VA_ARGS__
#define SHAREMIND_CLANGPR23383_WORKAROUND_CONSTEXPR_DEF(tmpName,...)
#define SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF(t,...)
#endif

#endif /* SHAREMIND_CLANGPR23383_H */
