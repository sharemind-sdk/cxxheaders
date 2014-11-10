/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCPR57887_H
#define SHAREMIND_GCCPR57887_H

#include "GccVersion.h"


/*
  This is a workaround around http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57887
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
#ifndef SHAREMIND_SILENCE_WORKAROUND_WARNINGS
#warning Using PR57887 workaround for g++ older than 4.9. Define \
         SHAREMIND_SILENCE_WORKAROUND_WARNINGS to silence.
#endif
#define SHAREMIND_GCCPR57887_PART1(...) __VA_ARGS__
#define SHAREMIND_GCCPR57887_PART2(...)
#else
#define SHAREMIND_GCCPR57887_PART1(...)
#define SHAREMIND_GCCPR57887_PART2(...) __VA_ARGS__
#endif

#endif // SHAREMIND_GCCPR57887_H
