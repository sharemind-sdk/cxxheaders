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

#ifndef SHAREMIND_INTEGER_DEMOTION_H
#define SHAREMIND_INTEGER_DEMOTION_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace sharemind {

namespace impl {

template <typename T>
struct size_in_bits {
    static constexpr std::size_t const value = sizeof (T) * 8u;
};

template <>
struct size_in_bits<bool> {
    static constexpr std::size_t const value = 1u;
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
    using void_if_disabled = void; // DO NOT DEFINE IN SPECIALIZATION
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
