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

#ifndef SHAREMIND_STRINGHASHER_H
#define SHAREMIND_STRINGHASHER_H

#include <boost/functional/hash.hpp>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>
#include "Concepts.h"
#include "Range.h"


namespace sharemind {

struct StringHasher {

    template <typename T>
    auto operator()(T && v) const noexcept
            -> SHAREMIND_REQUIRE_CONCEPTS_R(std::size_t,
                                            InputRangeTo(T, char),
                                            Not(DecaysTo(T, std::string)))
    {
        std::size_t seed = 0u;
        auto first(std::begin(v));
        auto last(std::end(v)); // Don't change to const, might break stuff
        for (; first != last; ++first)
            boost::hash_combine(seed, *first);
        return seed;
    }

    std::size_t operator()(std::string const & v) const noexcept
    { return boost::hash_range(v.begin(), v.end()); }

    std::size_t operator()(char const * const v) const noexcept
    { return boost::hash_range(v, v + std::strlen(v)); }

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_STRINGHASHER_H */
