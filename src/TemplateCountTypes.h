/*
 * Copyright (C) 2015-2017 Cybernetica
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

#ifndef SHAREMIND_TEMPLATECOUNTTYPES_H
#define SHAREMIND_TEMPLATECOUNTTYPES_H

#include <cstddef>
#include <type_traits>


namespace sharemind {

template <typename Needle, typename ... Haystack> struct TemplateCountTypes;

template <typename Needle>
struct TemplateCountTypes<Needle>: std::integral_constant<std::size_t, 0u> {};

template <typename Needle, typename T, typename ... Ts>
struct TemplateCountTypes<Needle, T, Ts...>
    : std::integral_constant<
        std::size_t,
        TemplateCountTypes<Needle, Ts...>::value
        + (std::is_same<Needle, T>::value ? 1u : 0u)>
{};

#if __cplusplus >= 201402L
template <typename Needle, typename ... Haystack>
constexpr auto TemplateCountTypes_v =
        TemplateCountTypes<Needle, Haystack...>::value;
#endif

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATECOUNTTYPES_H */
