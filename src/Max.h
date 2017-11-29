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

#ifndef SHAREMIND_MAX_H
#define SHAREMIND_MAX_H

#include <type_traits>
#include "TemplateAll.h"


namespace sharemind {
namespace Detail {

template <typename T>
constexpr T max(T v) noexcept { return v; }

template <typename T, typename ... Ts>
constexpr T max(T a, T b, Ts ... vs) noexcept
{ return (a > b) ? max(a, vs...) : max(b, vs...); }

} /* namespace Detail { */

template <typename ... Ts>
constexpr auto max(Ts && ... vs) ->
    typename std::enable_if<
        TemplateAll<
            std::is_arithmetic<
                typename std::decay<Ts>::type
            >::value...
        >::value,
        typename std::common_type<Ts...>::type
    >::type
{ return Detail::max(vs...); }

} /* namespace Sharemind { */

#endif /* SHAREMIND_MAX_H */
