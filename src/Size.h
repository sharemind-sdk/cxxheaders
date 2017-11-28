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

#ifndef SHAREMIND_SIZE_H
#define SHAREMIND_SIZE_H

#include <cstddef>


namespace sharemind {

template <typename T>
constexpr auto size(T const & t) noexcept(noexcept(t.size()))
        -> decltype(t.size())
{ return t.size(); }

template <typename T, std::size_t N>
constexpr inline std::size_t size(T const (&)[N]) noexcept { return N; }

} /* namespace Sharemind { */

#endif /* SHAREMIND_SIZE_H */
