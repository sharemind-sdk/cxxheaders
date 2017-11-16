/*
 * Copyright (C) 2017 Cybernetica
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

#include <cstring>
#include <locale>
#include <functional>
#include <type_traits>


namespace sharemind {
namespace Detail {

class StringAndCStringHasher {

private: /* Types: */

    struct MyCollate: std::collate<char> {};

public: /* Methods: */

    std::size_t operator()(std::string const & v) const noexcept {
        auto const h(m_collate->hash(v.c_str(), v.c_str() + v.size()));
        return std::hash<std::decay<decltype(h)>::type>()(h);
    }

    std::size_t operator()(char const * const str) const noexcept {
        auto const h(m_collate->hash(str, str + std::strlen(str)));
        return std::hash<std::decay<decltype(h)>::type>()(h);
    }

private: /* Fields: */

    std::shared_ptr<MyCollate> m_collate{std::make_shared<MyCollate>()};

};

struct StringAndCStringEqualTo {

    bool operator()(std::string const & a, std::string const & b)
            const noexcept
    { return a == b; }

    bool operator()(char const * const a, std::string const & b)
            const noexcept
    { return std::strcmp(a, b.c_str()) == 0; }

    bool operator()(std::string const & a, char const * const b)
            const noexcept
    { return std::strcmp(a.c_str(), b) == 0; }

    bool operator()(char const * const a, char const * const b)
            const noexcept
    { return std::strcmp(a, b) == 0; }

};

} /* namespace Detail { */

#define SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE \
    UnorderedMap< \
        std::string, \
        T, \
        Detail::StringAndCStringHasher, \
        Detail::StringAndCStringEqualTo, \
        Allocator \
    >

template <
    typename T,
    typename Allocator =
        typename UnorderedMap<
            std::string,
            T,
            Detail::StringAndCStringHasher,
            Detail::StringAndCStringEqualTo
        >::allocator_type
>
class SimpleUnorderedStringMap
        : public SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE
{

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

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::find;

    typename SimpleUnorderedStringMap::size_type count(char const * const key)
            const
    { return this->count(this->hash_function()(key), this->key_eq(), key); }

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::count;

    std::pair<typename SimpleUnorderedStringMap::iterator,
              typename SimpleUnorderedStringMap::iterator> equal_range(
            char const * const key)
    {
        return this->equal_range(this->hash_function()(key),
                                 this->key_eq(),
                                 key);
    }

    std::pair<typename SimpleUnorderedStringMap::const_iterator,
              typename SimpleUnorderedStringMap::const_iterator> equal_range(
            char const * const key) const
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

    using SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE::bucket;

};

#undef SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_BASE

} /* namespace Sharemind { */

#endif /* SHAREMIND_SIMPLEUNORDEREDSIMPLEMAP_H */
