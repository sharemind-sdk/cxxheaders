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

#ifndef SHAREMIND_MAKEUNIQUE_H
#define SHAREMIND_MAKEUNIQUE_H

#include <memory>
#include <type_traits>
#include <utility>


#if __cplusplus >= 201402L
#define SHAREMIND_MAKE_UNIQUE_DEPRECATED_NOTICE \
    [[deprecated("use std::make_unique instead")]]
#else
#define SHAREMIND_MAKE_UNIQUE_DEPRECATED_NOTICE
#endif

namespace sharemind {
namespace Detail {

template <typename T> struct MakeUnique { using regular = std::unique_ptr<T>; };

template <typename T>
struct MakeUnique<T[]> { using array = std::unique_ptr<T[]>; };

template <typename T, std::size_t N>
struct MakeUnique<T[N]> { using boundArray = void; };

} /* namespace Detail { */

template <typename T, typename ... Args>
SHAREMIND_MAKE_UNIQUE_DEPRECATED_NOTICE
typename Detail::MakeUnique<T>::regular makeUnique(Args && ... args)
{ return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }

template <typename T>
SHAREMIND_MAKE_UNIQUE_DEPRECATED_NOTICE
typename Detail::MakeUnique<T>::array makeUnique(std::size_t n)
{ return std::unique_ptr<T>(new typename std::remove_extent<T>::type[n]); }

template <typename T, typename ... Args>
SHAREMIND_MAKE_UNIQUE_DEPRECATED_NOTICE
typename Detail::MakeUnique<T>::boundArray makeUnique(Args && ...) = delete;

} /* namespace Sharemind { */

#undef SHAREMIND_MAKE_UNIQUE_DEPRECATED_NOTICE

#endif /* SHAREMIND_MAKEUNIQUE_H */
