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


namespace sharemind {

template <typename T>
constexpr T const & max(T const & a) noexcept { return a; }

template <typename T>
constexpr T const & max(T const & a, T const & b) noexcept
{ return a > b ? a : b; }

template <typename T, typename ... Args>
constexpr T const & max(T const & a,
                        T const & b,
                        Args const & ... args) noexcept
{ return max(a, max(b, args...)); }

} /* namespace Sharemind { */

#endif /* SHAREMIND_MAX_H */