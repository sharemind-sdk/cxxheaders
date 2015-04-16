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

#ifndef SHAREMIND_GCCISNOTHROWDESTRUCTIBLE_H
#define SHAREMIND_GCCISNOTHROWDESTRUCTIBLE_H

#include <sharemind/compiler-support/GccVersion.h>


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
