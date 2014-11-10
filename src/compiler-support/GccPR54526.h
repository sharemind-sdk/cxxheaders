/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCPR54526_H
#define SHAREMIND_GCCPR54526_H

/*
  This is a workaround around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54526
  which parses <:: as a digraph in code like:

    template <> struct S<::A> {};

  Use this workaround as follows:

    template <> struct S<SHAREMIND_GCCPR54526::A> {};
*/

#define SHAREMIND_GCCPR54526

#endif // SHAREMIND_GCCPR54526_H
