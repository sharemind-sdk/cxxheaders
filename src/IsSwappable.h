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

#ifndef SHAREMIND_ISSWAPPABLE_H
#define SHAREMIND_ISSWAPPABLE_H

#include <type_traits>
#include <utility>
#include "IsReferenceable.h"


namespace sharemind {
namespace Detail {
namespace IsSwappable {

using std::swap;

template <typename T, typename U = T,
          bool valid = !std::is_void<T>::value && !std::is_void<U>::value>
struct Test {
    template <typename T_, typename U_,
              typename = decltype(swap(std::declval<T_>(), std::declval<U_>()))>
    static std::true_type test(int);
    template <typename, typename> static std::false_type test(...);

    static constexpr bool const value = decltype(test<T, U>(0))::value
                                     && decltype(test<U, T>(0))::value;
};

template <typename T, typename U>
struct Test<T, U, false> : std::false_type {};

template <typename T, typename U = T, bool swappable = Test<T, U>::value>
struct TestNoexcept
        : std::integral_constant<
            bool,
               noexcept(swap(std::declval<T>(), std::declval<U>()))
            && noexcept(swap(std::declval<U>(), std::declval<T>()))>
{};

template <typename T, typename U>
struct TestNoexcept<T, U, false> : std::false_type {};

} /* namespace IsSwappable { */
} /* namespace Detail { */

template <typename T, typename U>
struct IsSwappableWith
        : std::integral_constant<bool, Detail::IsSwappable::Test<T, U>::value>
{};

template <typename T>
struct IsSwappable
        : std::integral_constant<
            bool,
            std::conditional<
                IsReferenceable<T>::value,
                IsSwappableWith<T &, T &>,
                std::false_type
            >::type::value>
{};

template <typename T, typename U>
struct IsNothrowSwappableWith
        : std::integral_constant<
            bool,
            Detail::IsSwappable::TestNoexcept<T, U>::value>
{};

template <typename T>
struct IsNothrowSwappable
        : std::integral_constant<
            bool,
            std::conditional<
                IsReferenceable<T>::value,
                IsNothrowSwappableWith<T &, T &>,
                std::false_type
            >::type::value>
{};

} /* namespace Sharemind { */

#endif /* SHAREMIND_ISSWAPPABLE_H */
