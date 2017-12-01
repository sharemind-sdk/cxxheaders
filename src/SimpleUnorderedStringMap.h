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

#ifndef SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_H
#define SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_H

#include "UnorderedMap.h"
#include <string>

#include <boost/functional/hash.hpp>
#include <cassert>
#include <cstring>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include "Concepts.h"
#if __cplusplus < 201402L
#include "IntegralComparisons.h"
#endif
#include "Range.h"


namespace sharemind {
namespace Detail {
namespace SimpleUnorderedStringMap {

struct Hasher {

    template <typename T>
    auto operator()(T && v) const noexcept
            -> SHAREMIND_REQUIRE_CONCEPTS_R(std::size_t, InputRangeTo(T, char))
    { return boost::hash_range(std::begin(v), std::end(v)); }

    std::size_t operator()(char const * const v) const noexcept
    { return boost::hash_range(v, v + std::strlen(v)); }

};

struct KeyEqual {

    template <typename T>
    auto operator()(std::string const & a, T && b) const noexcept
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool,
                                        RandomAccessRangeTo(T, char),
                                        BoundedRange(T))
    {
        auto bIt(std::begin(b));
        auto const bEnd(std::end(b));
        if (!integralEqual(a.size(), std::distance(bIt, bEnd)))
            return false;
        return std::equal(bIt, bEnd, a.begin());
    }

    template <typename T>
    auto operator()(std::string const & a, T && b) const noexcept
            -> SHAREMIND_REQUIRE_CONCEPTS_R(
                    bool,
                    InputRangeTo(T, char),
                    Not(All(RandomAccessRange(T), BoundedRange(T)))
            )
    {
        auto aIt(a.begin());
        auto const aEnd(a.end());
        auto bIt(std::begin(b));
        auto const bEnd(std::end(b));
        for (; aIt != aEnd; ++aIt, ++bIt)
            if ((bIt == bEnd) || (*aIt != *bIt))
                return false;
        return bIt == bEnd;
    }

    template <typename T>
    auto operator()(T && a, std::string const & b) const noexcept
            -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InputRangeTo(T, char))
    { return this->operator()(b, std::forward<T>(a)); }

    bool operator()(std::string const & a, std::string const & b)
            const noexcept
    { return a == b; }

    bool operator()(char const * const a, std::string const & b)
            const noexcept
    { return std::strcmp(a, b.c_str()) == 0; }

    bool operator()(std::string const & a, char const * const b)
            const noexcept
    { return std::strcmp(a.c_str(), b) == 0; }

};

} /* namespace SimpleUnorderedStringMap { */
} /* namespace Detail { */

#define SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE \
    UnorderedMap< \
        std::string, \
        T, \
        Detail::SimpleUnorderedStringMap::Hasher, \
        Detail::SimpleUnorderedStringMap::KeyEqual, \
        Allocator \
    >

template <
    typename T,
    typename Allocator =
        typename UnorderedMap<
            std::string,
            T,
            Detail::SimpleUnorderedStringMap::Hasher,
            Detail::SimpleUnorderedStringMap::KeyEqual
        >::allocator_type
>
class SimpleUnorderedStringMap
        : public SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE
{

public: /* Types: */

    using EqualRangeReturnType =
            std::pair<typename SimpleUnorderedStringMap::iterator,
                      typename SimpleUnorderedStringMap::iterator>;

    using EqualRangeConstReturnType =
            std::pair<typename SimpleUnorderedStringMap::const_iterator,
                      typename SimpleUnorderedStringMap::const_iterator>;

public: /* Methods: */

    /* Construct/copy/destroy: */

    SimpleUnorderedStringMap() = default;

    SimpleUnorderedStringMap(
            typename SimpleUnorderedStringMap::size_type numBuckets)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(numBuckets)
    {}

    SimpleUnorderedStringMap(
            typename SimpleUnorderedStringMap::size_type numBuckets,
            typename SimpleUnorderedStringMap::allocator_type const & alloc)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(numBuckets, alloc)
    {}

    template <typename InputIterator>
    SimpleUnorderedStringMap(InputIterator first, InputIterator last)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(first, last)
    {}

    template <typename InputIterator>
    SimpleUnorderedStringMap(
            InputIterator first,
            InputIterator last,
            typename SimpleUnorderedStringMap::size_type numBuckets)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(first, last, numBuckets)
    {}

    template <typename InputIterator>
    SimpleUnorderedStringMap(
            InputIterator first,
            InputIterator last,
            typename SimpleUnorderedStringMap::size_type numBuckets,
            typename SimpleUnorderedStringMap::allocator_type const & alloc)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(first, last, numBuckets, alloc)
    {}

    SimpleUnorderedStringMap(
            std::initializer_list<typename SimpleUnorderedStringMap::value_type>
                    values)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(std::move(values))
    {}

    SimpleUnorderedStringMap(
            std::initializer_list<typename SimpleUnorderedStringMap::value_type>
                    values,
            typename SimpleUnorderedStringMap::size_type numBuckets)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(std::move(values), numBuckets)
    {}

    SimpleUnorderedStringMap(
            std::initializer_list<typename SimpleUnorderedStringMap::value_type>
                    values,
            typename SimpleUnorderedStringMap::size_type numBuckets,
            typename SimpleUnorderedStringMap::allocator_type const & a)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(std::move(values),
                                                  numBuckets,
                                                  a)
    {}

    SimpleUnorderedStringMap(SimpleUnorderedStringMap const &) = default;

    SimpleUnorderedStringMap(SimpleUnorderedStringMap &&) = default;

    explicit SimpleUnorderedStringMap(
            typename SimpleUnorderedStringMap::allocator_type const & allocator)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(allocator)
    {}

    SimpleUnorderedStringMap(
            SimpleUnorderedStringMap const & copy,
            typename SimpleUnorderedStringMap::allocator_type const & allocator)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(copy, allocator)
    {}

    SimpleUnorderedStringMap(
            SimpleUnorderedStringMap && move,
            typename SimpleUnorderedStringMap::allocator_type const & allocator)
        : SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE(std::move(move), allocator)
    {}


    /* Modifiers: */

    SimpleUnorderedStringMap & operator=(SimpleUnorderedStringMap const &)
            = default;

    SimpleUnorderedStringMap & operator=(SimpleUnorderedStringMap &&) = default;

    typename SimpleUnorderedStringMap::size_type erase(char const * const key) {
        auto it(find(key));
        return (it != this->end()) ? this->erase(std::move(it)) : 0u;
    }

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::erase;


    /* Lookup: */

    typename SimpleUnorderedStringMap::iterator find(char const * const key)
    { return find(this->hash_function()(key), this->key_eq(), key); }

    typename SimpleUnorderedStringMap::const_iterator find(
            char const * const key) const
    { return find(this->hash_function()(key), this->key_eq(), key); }

    template <typename Range>
    auto find(Range && key) const
            -> SHAREMIND_REQUIRE_CONCEPTS_R(
                    typename SimpleUnorderedStringMap::const_iterator,
                    InputRangeTo(Range, char),
                    Not(DecaysTo(Range, std::string))
                )
    { return find(this->hash_function()(key), this->key_eq(), key); }

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::find;

    typename SimpleUnorderedStringMap::size_type count(char const * const key)
            const
    { return this->count(this->hash_function()(key), this->key_eq(), key); }

    template <typename Range>
    auto count(Range && key) const
            -> SHAREMIND_REQUIRE_CONCEPTS_R(
                    typename SimpleUnorderedStringMap::size_type,
                    InputRangeTo(Range, char),
                    Not(DecaysTo(Range, std::string))
                )
    { return this->count(this->hash_function()(key), this->key_eq(), key); }

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::count;

    EqualRangeReturnType equal_range(char const * const key) {
        return this->equal_range(this->hash_function()(key),
                                 this->key_eq(),
                                 key);
    }

    EqualRangeConstReturnType equal_range(char const * const key) const {
        return this->equal_range(this->hash_function()(key),
                                 this->key_eq(),
                                 key);
    }

    template <typename Range>
    auto equal_range(Range && key)
            -> SHAREMIND_REQUIRE_CONCEPTS_R(EqualRangeReturnType,
                                            InputRangeTo(Range, char),
                                            Not(DecaysTo(Range, std::string)))
    {
        return this->equal_range(this->hash_function()(key),
                                 this->key_eq(),
                                 key);
    }

    template <typename Range>
    auto equal_range(Range && key) const
        -> SHAREMIND_REQUIRE_CONCEPTS_R(EqualRangeConstReturnType,
                                        InputRangeTo(Range, char),
                                        Not(DecaysTo(Range, std::string)))
    {
        return this->equal_range(this->hash_function()(key),
                                 this->key_eq(),
                                 key);
    }

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::equal_range;


    /* Bucket interface: */

    typename SimpleUnorderedStringMap::size_type bucket(char const * const key)
            const
    { return this->bucket(this->hash_function()(key)); }

    template <typename Range>
    auto bucket(Range && key) const
            -> SHAREMIND_REQUIRE_CONCEPTS_R(
                    typename SimpleUnorderedStringMap::size_type,
                    InputRangeTo(Range, char),
                    Not(DecaysTo(Range, std::string)))
    { return this->bucket(this->hash_function()(key)); }

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::bucket;

};

#undef SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE

} /* namespace Sharemind { */

#endif /* SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_H */
