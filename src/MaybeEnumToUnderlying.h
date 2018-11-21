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

#ifndef SHAREMIND_MAYBEENUMTOUNDERLYING_H
#define SHAREMIND_MAYBEENUMTOUNDERLYING_H

#include <type_traits>
#include <utility>
#include "EnumToUnderlying.h"


namespace sharemind {
namespace Detail {
namespace MaybeEnumToUnderlying {

template <bool> struct Impl {
    template <typename T>
    constexpr static T get(T v) noexcept { return v; }
};

template <> struct Impl<true> {
    template <typename T>
    constexpr static auto get(T && v) noexcept
    { return enumToUnderlying(std::forward<T>(v)); }

};

} /* namespace MaybeEnumToUnderlying { */
} /* namespace Detail { */

template <typename T>
constexpr auto maybeEnumToUnderlying(T && v) noexcept {
    return Detail::MaybeEnumToUnderlying::Impl<
        std::is_enum<typename std::decay<T>::type>::value>::get(
            std::forward<T>(v));
}

} /* namespace sharemind { */

#endif /* SHAREMIND_MAYBEENUMTOUNDERLYING_H */
