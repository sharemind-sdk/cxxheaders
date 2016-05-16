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

#ifndef SHAREMIND_FUNCTIONTRAITS_H
#define SHAREMIND_FUNCTIONTRAITS_H

#include <cstddef>
#include <tuple>
#include <type_traits>
#include "EnumConstant.h"


namespace sharemind {

template <typename F>
struct FunctionTraits;

template <typename R, typename ... Args>
struct FunctionTraits<R(Args...)> {
  using return_type = R;
  SHAREMIND_ENUMCONSTANT(std::size_t, arity, sizeof...(Args));
  template <std::size_t N> struct argument {
    static_assert(N < arity, "invalid parameter index");
    using type = typename std::tuple_element<N, std::tuple<Args...> >::type;
  };
};

template <typename R, typename ... Args>
struct FunctionTraits<R(*)(Args...)>: FunctionTraits<R(Args...)> {};

template <typename C, typename R, typename ... Args>
struct FunctionTraits<R(C::*)(Args...)>: FunctionTraits<R(C *,Args...)> {};

template <typename C, typename R, typename ... Args>
struct FunctionTraits<R(C::*)(Args...) const>
    : FunctionTraits<R(C const *,Args...)> {};

template <typename C, typename R, typename ... Args>
struct FunctionTraits<R(C::*)(Args...) volatile>
    : FunctionTraits<R(C volatile *,Args...)> {};

template <typename C, typename R, typename ... Args>
struct FunctionTraits<R(C::*)(Args...) const volatile>
    : FunctionTraits<R(C const volatile *,Args...)> {};

template <typename F> struct FunctionTraits<F &>: FunctionTraits<F> {};

template <typename F> struct FunctionTraits<const F &>
    : FunctionTraits<const F> {};

template <typename F> struct FunctionTraits<volatile F &>
    : FunctionTraits<volatile F> {};

template <typename F> struct FunctionTraits<const volatile F &>
    : FunctionTraits<const volatile F> {};

template <typename F> struct FunctionTraits<F &&>: FunctionTraits<F> {};

template <typename F> struct FunctionTraits<const F &&>
    : FunctionTraits<const F> {};

template <typename F> struct FunctionTraits<volatile F &&>
    : FunctionTraits<volatile F> {};

template <typename F> struct FunctionTraits<const volatile F &&>
    : FunctionTraits<const volatile F> {};

template <typename F>
struct FunctionTraits: FunctionTraits<decltype(&F::operator())> {};

} /* namespace sharemind { */

#endif /* SHAREMIND_FUNCTIONTRAITS_H */
