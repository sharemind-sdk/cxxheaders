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

#ifndef SHAREMIND_GCCPR57887_H
#define SHAREMIND_GCCPR57887_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  This is a workaround around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=57887
  which prevents code like the following to parse with GCC versions before 4.9.

  struct Outer {
    template <typename>
    struct Inner {
      enum Type { TYPE_1 };
      Type m_type = TYPE_1;
    };
  };
  // or
  struct Outer {
    template <int N>
    struct Inner {
      int m_value = N;
    };
  };

  Such code resulted in NULL pointer dereferences in GCC 4.7 and errors in 4.8.

  Use this workaround as follows:
  struct Outer {
    template <typename>
    struct Inner {
      enum Type { TYPE_1 };
      Inner() SHAREMIND_GCCPR57887_PART1(: m_type(TYPE_1)) {}
      Type m_type SHAREMIND_GCCPR57887_PART2(= TYPE_1);
    };
  };
*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40900)
#define SHAREMIND_GCCPR57887_PART1(...) __VA_ARGS__
#define SHAREMIND_GCCPR57887_PART2(...)
#else
#define SHAREMIND_GCCPR57887_PART1(...)
#define SHAREMIND_GCCPR57887_PART2(...) __VA_ARGS__
#endif

#endif // SHAREMIND_GCCPR57887_H
