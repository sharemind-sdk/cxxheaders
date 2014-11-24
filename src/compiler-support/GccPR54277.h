/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCPR54277_H
#define SHAREMIND_GCCPR54277_H

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
