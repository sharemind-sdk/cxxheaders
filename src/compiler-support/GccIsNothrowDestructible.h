/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCISNOTHROWDESTRUCTIBLE_H
#define SHAREMIND_GCCISNOTHROWDESTRUCTIBLE_H

#include "GccVersion.h"

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
#include <type_traits>
#include <utility>
namespace sharemind {
namespace workaround {
struct is_nothrow_destructible_helper {
  template<typename _Tp>
  static std::integral_constant<bool, noexcept(std::declval<_Tp&>().~_Tp())>
  test(int);

  template<typename> static std::false_type test(...);
};

template<typename T>
struct is_nothrow_destructible
    : std::integral_constant<
        bool,
        !(std::is_void<T>::value
          || (std::is_array<T>::value && !std::extent<T>::value)
          || std::is_function<T>::value)
        && (std::is_reference<T>::value
            || std::is_scalar<T>::value
            || decltype(is_nothrow_destructible_helper::test<
                            typename std::remove_reference<
                                    typename std::remove_all_extents<T>::type>
                                ::type>(0))::value)
      >::type {};

} /* namespace workaround { */
} /* namespace sharemind { */
namespace std {
using sharemind::workaround::is_nothrow_destructible;
} /* namespace std { */
#endif

#endif /* SHAREMIND_GCCISNOTHROWDESTRUCTIBLE_H */
