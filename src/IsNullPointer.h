/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_ISNULLPOINTER_H
#define SHAREMIND_ISNULLPOINTER_H

#include <type_traits>


namespace sharemind {

#if __cplusplus >= 201402L

template <typename T>
using IsNullPointer = ::std::is_null_pointer<T>;

#else

/*
  This is the C++11 equivalent of

      template <typename T> struct ::std::is_null_pointer;

  in C++14 (LWG 2247)
*/
template <typename T>
using IsNullPointer =
        ::std::is_same<typename ::std::decay<T>::type, decltype(nullptr)>;

#endif

} /* namespace Sharemind { */

#endif /* SHAREMIND_ISNULLPOINTER_H */
