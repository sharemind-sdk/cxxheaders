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

#ifndef SHAREMIND_TOUNSIGNED_H
#define SHAREMIND_TOUNSIGNED_H

#include <type_traits>
#include "RemoveCvref.h"


namespace sharemind {
namespace Detail {

template <typename T>
using ToUnsigned =
    std::conditional<
        std::is_signed<RemoveCvrefT<T> >::value,
        typename std::make_unsigned<RemoveCvrefT<T> >::type,
        T const &
    >;

template <typename T>
using ToUnsigned_t = typename ToUnsigned<T>::type;

} /* namespace Detail { */

template <typename T>
constexpr
Detail::ToUnsigned_t<T> toUnsigned(T const & v) noexcept
{ return static_cast<Detail::ToUnsigned_t<T> >(v); }

} /* namespace Sharemind { */

#endif /* SHAREMIND_TOUNSIGNED_H */
