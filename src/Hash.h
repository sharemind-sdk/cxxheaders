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

#ifndef SHAREMIND_HASH_H
#define SHAREMIND_HASH_H

#include <cstddef>

#include <boost/functional/hash/hash.hpp>
#include <type_traits>
#include <utility>
#include "PotentiallyVoidTypeInfo.h"


namespace sharemind {

using HashValue = std::size_t;

inline HashValue hashCombineRawData(HashValue seed,
                                    void const * const data,
                                    std::size_t const size) noexcept
{
    boost::hash_range(seed,
                      static_cast<char const *>(data),
                      static_cast<char const *>(ptrAdd(data, size)));
    return seed;
}

template <typename ... Args>
inline HashValue hashCombineRawData(HashValue seed,
                                    void const * const data,
                                    std::size_t const size,
                                    Args && ... args) noexcept
{
    return hashCombineRawData(hashCombineRawData(seed, data, size),
                              std::forward<Args>(args)...);
}

template <typename ... Args>
inline HashValue hashRawData(Args && ... args) noexcept
{ return hashCombineRawData(0u, std::forward<Args>(args)...); }

struct HashMemberCaller {
    template <typename T>
    auto operator()(T const & t) const noexcept(noexcept(t.hash()))
    { return t.hash(); }
};

template <typename T>
struct HashMemberCallerT {
    auto operator()(T const & t) const noexcept(noexcept(t.hash()))
    { return t.hash(); }
};

struct RawHasher {
    template <typename T>
    HashValue operator()(T const & t) const noexcept
    { return hashRawData(&t, sizeof(t)); }
};

template <typename T>
struct RawHasherT {
    HashValue operator()(T const & t) const noexcept
    { return hashRawData(&t, sizeof(t)); }
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_HASH_H */
