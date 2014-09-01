/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_FUNCTIONTRAITS_H
#define SHAREMIND_FUNCTIONTRAITS_H

#include <cstddef>
#include <tuple>
#include <type_traits>


namespace sharemind {

template <typename F>
struct FunctionTraits;

template <typename R, typename ... Args>
struct FunctionTraits<R(Args...)> {
  using return_type = R;
  static constexpr std::size_t arity = sizeof...(Args);
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
