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

#ifndef SHAREMIND_ISPOWEROFTWO_H
#define SHAREMIND_ISPOWEROFTWO_H

#include <type_traits>


namespace sharemind {

template <typename T>
inline constexpr auto isPowerOfTwo(T const x) noexcept
        -> typename std::enable_if<std::is_integral<T>::value, bool>::type
{ return (x > 0) && ((x & (x - 1)) == 0); }

} /* namespace Sharemind { */

#endif /* SHAREMIND_ISPOWEROFTWO_H */
