/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_INTEGER_DEMOTION_H
#define SHAREMIND_INTEGER_DEMOTION_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace sharemind {

namespace impl {

template <typename T>
struct size_in_bits {
    static constexpr size_t value = sizeof (T) * 8u;
};

template <>
struct size_in_bits<bool> {
    static constexpr size_t value = 1u;
};

// Feel free to loosen the restrictions here
template <typename T, typename U>
struct __attribute__ ((visibility("internal"))) can_demote_primitives
    : std::integral_constant<bool,
        std::is_integral<T>::value &&
        std::is_integral<U>::value &&
        (size_in_bits<T>::value < size_in_bits<U>::value)
      >
{ };

template <typename T, typename U, class Enable = void> struct __attribute__ ((visibility("internal"))) demote_integer_impl {
    typedef void void_if_disabled; // DO NOT DEFINE IN SPECIALIZATION
};

template <typename T, typename U>
struct __attribute__ ((visibility("internal"))) demote_integer_impl<T, U, typename std::enable_if<impl::can_demote_primitives<T, U>::value >::type> {
    static inline T demote (const U x) { return x; }
};

// If
template <typename T, typename U, class Enabled = void> struct can_demote : public std::true_type { };
template <typename T, typename U> struct can_demote<T, U, typename demote_integer_impl<T, U>::void_if_disabled> : public std::false_type { };

} // namespace impl {

template <typename T, typename U>
inline T demote_integer (const U x, void*) __attribute__ ((visibility("internal")));

template <typename T, typename U>
inline T demote_integer (const U x, typename std::enable_if<impl::can_demote<T, U>::value >::type* = 0) {
    return impl::demote_integer_impl<T, U>::demote (x);
}

} /* namespace sharemind { */

#endif /* SHAREMIND_INTEGER_DEMOTION_H */
