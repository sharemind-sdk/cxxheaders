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

#ifndef SHAREMIND_MAX_H
#define SHAREMIND_MAX_H

#include <type_traits>
#include <utility>
#include "MaybeEnumToUnderlying.h"


namespace sharemind {

template <typename T> constexpr auto max(T && a) noexcept ->
        decltype(maybeEnumToUnderlying(std::forward<T>(a)))
{ return maybeEnumToUnderlying(std::forward<T>(a)); }

template <typename T1, typename T2>
constexpr auto max(T1 && a, T2 && b) noexcept
    -> decltype(maybeEnumToUnderlying(a)
                > maybeEnumToUnderlying(b)
                ? maybeEnumToUnderlying(std::forward<T1>(a))
                : maybeEnumToUnderlying(std::forward<T2>(b)))
{
    return maybeEnumToUnderlying(a) > maybeEnumToUnderlying(b)
           ? maybeEnumToUnderlying(std::forward<T1>(a))
           : maybeEnumToUnderlying(std::forward<T2>(b));
}

template <typename T1, typename T2, typename ... Args>
constexpr auto max(T1 && a, T2 && b, Args && ... args) noexcept
    -> decltype(max(max(std::forward<T1>(a),
                        std::forward<T2>(b)),
                    std::forward<Args>(args)...))
{
    return max(max(std::forward<T1>(a), std::forward<T2>(b)),
               std::forward<Args>(args)...);
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_MAX_H */
