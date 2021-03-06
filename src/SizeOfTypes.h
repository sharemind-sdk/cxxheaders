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

#ifndef SHAREMIND_SIZEOFTYPES_H
#define SHAREMIND_SIZEOFTYPES_H

#include <cstddef>
#include <type_traits>


namespace sharemind {

template <typename ... Ts> struct SizeOfTypes;

template <> struct SizeOfTypes<>: std::integral_constant<std::size_t, 0u> {};

template <typename T, typename ... Ts>
struct SizeOfTypes<T, Ts...>
        : std::integral_constant<std::size_t,
                                 sizeof(T) + SizeOfTypes<Ts...>::value>
{};

template <typename ... Ts>
constexpr std::size_t sizeOfTypes() noexcept
{ return SizeOfTypes<Ts...>::value; }

} /* namespace sharemind { */

#endif /* SHAREMIND_SIZEOFTYPES_H */
