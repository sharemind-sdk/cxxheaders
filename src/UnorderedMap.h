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

#ifndef SHAREMIND_UNORDEREDMAP_H
#define SHAREMIND_UNORDEREDMAP_H

/**
  \file

  \todo Write missing tests.
*/

#include <algorithm>
#include <boost/iterator/transform_iterator.hpp>
#include <functional>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include "Concepts.h"
#include "Void.h"


namespace sharemind {

/**
    \brief Similar to std::unordered_map, except that it allows explicit lookup,
           manipulation etc by using hashes and predicates.
    \todo Verify all requirements for unordered associative containers are met.
*/
template <
    typename Key,
    typename T,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>,
    typename Allocator = std::allocator<std::pair<const Key, T> >
>
class UnorderedMap {

public: /* Types: */

    using key_type = Key;
    using value_type = std::pair<Key const, T>;
    using mapped_type = T;
    using hasher = Hash;
    using hash_type =
            decltype(
                    std::declval<hasher &>()(std::declval<key_type const &>()));
    using key_equal = KeyEqual;
    using allocator_type = Allocator;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;

private: /* Types: */

    using InnerContainer =
            std::unordered_multimap<
                hash_type,
                std::shared_ptr<value_type>
            >;

    struct InnerIteratorTransformer {
        reference operator()(typename InnerContainer::reference const & r)
                const noexcept
        { return *r.second; }

        const_reference operator()(
                typename InnerContainer::const_reference const & r)
                const noexcept
        { return *r.second; }
    };

public: /* Types: */

    using size_type = typename InnerContainer::size_type;
    using difference_type = typename InnerContainer::difference_type;

    using iterator =
            boost::transform_iterator<
                InnerIteratorTransformer,
                typename InnerContainer::iterator,
                reference,
                value_type
            >;

    using const_iterator =
            boost::transform_iterator<
                InnerIteratorTransformer,
                typename InnerContainer::const_iterator,
                const_reference,
                value_type
            >;

    using local_iterator =
            boost::transform_iterator<
                InnerIteratorTransformer,
                typename InnerContainer::local_iterator,
                reference,
                value_type
            >;

    using const_local_iterator =
            boost::transform_iterator<
                InnerIteratorTransformer,
                typename InnerContainer::const_local_iterator,
                const_reference,
                value_type
            >;

public: /* Methods: */

    /* Construct/copy/destroy: */

    UnorderedMap() = default;

    explicit UnorderedMap(size_type numBuckets,
                          hasher const & hf = hasher(),
                          key_equal const & eql = key_equal(),
                          allocator_type const & a = allocator_type())
        : m_hasher(hf)
        , m_pred(eql)
        , m_container(numBuckets)
        , m_allocator(a)
    {}

    UnorderedMap(size_type numBuckets,
                 allocator_type const & alloc)
        : UnorderedMap(numBuckets, hasher(), key_equal(), alloc)
    {}


    UnorderedMap(size_type numBuckets,
                 hasher const & hf,
                 allocator_type const & alloc)
        : UnorderedMap(numBuckets, hf, key_equal(), alloc)
    {}

    template <typename InputIterator>
    UnorderedMap(InputIterator first,
                 InputIterator last,
                 size_type numBuckets = InnerContainer().bucket_count(),
                 hasher const & hf = hasher(),
                 key_equal const & eql = key_equal(),
                 allocator_type const & a = allocator_type())
        : UnorderedMap(numBuckets, hf, eql, a)
    { insert(first, last); }

    template <typename InputIterator>
    UnorderedMap(InputIterator first,
                 InputIterator last,
                 size_type numBuckets,
                 allocator_type const & alloc)
        : UnorderedMap(std::move(first),
                       std::move(last),
                       numBuckets,
                       hasher(),
                       key_equal(),
                       alloc)
    {}

    template <typename InputIterator>
    UnorderedMap(InputIterator first,
                 InputIterator last,
                 size_type numBuckets,
                 hasher const & hf,
                 allocator_type const & alloc)
        : UnorderedMap(std::move(first),
                       std::move(last),
                       numBuckets,
                       hf,
                       key_equal(),
                       alloc)
    {}

    UnorderedMap(std::initializer_list<value_type> values,
                 size_type numBuckets = InnerContainer().bucket_count(),
                 hasher const & hf = hasher(),
                 key_equal const & eql = key_equal(),
                 allocator_type const & a = allocator_type())
        : UnorderedMap(numBuckets, hf, eql, a)
    { insert(values); }

    UnorderedMap(std::initializer_list<value_type> values,
                 size_type numBuckets,
                 allocator_type const & alloc)
        : UnorderedMap(std::move(values),
                       numBuckets,
                       hasher(),
                       key_equal(),
                       alloc)
    {}

    UnorderedMap(std::initializer_list<value_type> values,
                 size_type numBuckets,
                 hasher const & hf,
                 allocator_type const & alloc)
        : UnorderedMap(std::move(values),
                       numBuckets,
                       hf,
                       key_equal(),
                       alloc)
    {}

    UnorderedMap(UnorderedMap const & copy)
        : m_hasher(copy.m_hasher)
        , m_pred(copy.m_pred)
        , m_container(copy.m_container)
        , m_allocator(
            std::allocator_traits<allocator_type>
                ::select_on_container_copy_construction(copy.m_allocator))
    {}

    UnorderedMap(UnorderedMap && move) = default;

    explicit UnorderedMap(allocator_type const & allocator)
        : m_allocator(allocator)
    {}

    UnorderedMap(UnorderedMap const & copy, allocator_type const & allocator)
        : m_hasher(copy.m_hasher)
        , m_pred(copy.m_pred)
        , m_container(copy.m_container)
        , m_allocator(
            std::allocator_traits<allocator_type>
                ::select_on_container_copy_construction(allocator))
    {}

    UnorderedMap(UnorderedMap && move, allocator_type const & allocator)
        : m_hasher(std::move(move.m_hasher))
        , m_pred(std::move(move.m_pred))
        , m_container(std::move(move.m_container))
        , m_allocator(allocator)
    {}

    ~UnorderedMap() noexcept {}

    UnorderedMap & operator=(UnorderedMap const & rhs) {
        if (&rhs != this) {
            m_hasher = rhs.m_hasher;
            m_pred = rhs.m_pred;
            m_container = rhs.m_container;
            m_allocator =
                    std::allocator_traits<allocator_type>
                          ::select_on_container_copy_construction(
                                rhs.m_allocator);
        }
        return *this;
    }

    UnorderedMap & operator=(UnorderedMap && rhs) = default;

    UnorderedMap & operator=(std::initializer_list<value_type> values) {
        InnerContainer newInner;
        for (auto & v : values)
            newInner.emplace(std::allocate_shared<value_type>(m_allocator,
                                                              std::move(v)));
        newInner.rehash();
        std::swap(m_container, newInner);
        return *this;
    }

    allocator_type get_allocator() const noexcept { return m_allocator; }


    /* Size and capacity: */

    bool empty() const
            noexcept(
                noexcept(std::declval<InnerContainer const &>().empty()))
    { return m_container.empty(); }

    size_type size() const
            noexcept(
                noexcept(std::declval<InnerContainer const &>().size()))
    { return m_container.size(); }

    size_type max_size() const
        noexcept(
            noexcept(std::declval<InnerContainer const &>().max_size()))
    { return m_container.max_size(); }


    /* Iterators: */

    iterator begin() noexcept
    { return iterator(m_container.begin()); }

    const_iterator begin() const noexcept
    { return const_iterator(m_container.cbegin()); }

    iterator end() noexcept
    { return iterator(m_container.end()); }

    const_iterator end() const noexcept
    { return const_iterator(m_container.cend()); }

    const_iterator cbegin() const noexcept
    { return const_iterator(m_container.cbegin()); }

    const_iterator cend() const noexcept
    { return const_iterator(m_container.cend()); }


    /* Modifiers: */

    template <typename ... Args>
    std::pair<iterator, bool> emplace(Args && ... args) {
        using R = std::pair<iterator, bool>;
        auto innerValue(std::allocate_shared<value_type>(
                            m_allocator,
                            std::forward<Args>(args)...));
        auto const hash(m_hasher(innerValue->first));
        for (auto er = m_container.equal_range(hash);
             er.first != er.second;
             ++er.first)
            if (m_pred(innerValue->first, er.first->second->first))
                return R(iterator(std::move(er.first)), false);
        return R(iterator(m_container.emplace(std::move(hash),
                                              std::move(innerValue))),
                 true);
    }

    template <typename ... Args>
    iterator emplace_hint(const_iterator /* hint */, Args && ... args)
    { return emplace(std::forward<Args>(args)...).first; }

    std::pair<iterator, bool> insert(value_type const & obj)
    { return emplace(obj); }

    template <typename P,
              SHAREMIND_REQUIRES_CONCEPTS(Constructible(value_type, P &&))>
    std::pair<iterator, bool> insert(P && obj)
    { return emplace(std::forward<P>(obj)); }

    iterator insert(const_iterator hint, value_type const & obj)
    { return emplace_hint(std::move(hint), obj); }

    template <typename P,
              SHAREMIND_REQUIRES_CONCEPTS(Constructible(value_type, P &&))>
    iterator insert(const_iterator hint, P && obj)
    { return emplace_hint(std::move(hint), std::forward<P>(obj)); }

    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last) {
        using V = typename std::iterator_traits<InputIterator>::value_type;
        std::for_each(first, last, [this](V const & v) { insert(v); });
    }

    void insert(std::initializer_list<value_type> values)
    { return insert(values.begin(), values.end()); }

    iterator erase(const_iterator position)
    { return iterator(m_container.erase(position.base())); }

    size_type erase(key_type const & k) {
        for (auto er = m_container.equal_range(m_hasher(k));
             er.first != er.second;
             ++er.first)
            if (m_pred(k, er.first->second->first)) {
                m_container.erase(er.first);
                return 1u;
            }
        return 0u;
    }

    iterator erase(const_iterator first, const_iterator last)
    { return iterator(m_container.erase(first.base(), last.base())); }

    void clear() noexcept { m_container.clear(); }

    void swap(UnorderedMap & other) {
        std::swap(m_hasher, other.m_hasher);
        std::swap(m_pred, other.m_pred);
        std::swap(m_container, other.m_container);
        std::swap(m_allocator, other.m_allocator);
    }


    /* Observers: */

    hasher hash_function() const
        noexcept(noexcept(std::is_nothrow_copy_constructible<hasher>::value))
    { return m_hasher; }

    key_equal key_eq() const
        noexcept(noexcept(std::is_nothrow_copy_constructible<key_equal>::value))
    { return m_pred; }


    /* Lookup: */

    iterator find(key_type const & key) {
        auto const hash(m_hasher(key));
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return iterator(er.first);
        return end();
    }

    const_iterator find(key_type const & key) const {
        auto const hash(m_hasher(key));
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return const_iterator(er.first);
        return end();
    }

    /** \note not in std::unordered_map */
    template <typename Key_,
              SHAREMIND_REQUIRES_CONCEPTS(
                    Not(UnaryPredicate(Key_, key_type const &)))>
    iterator find(hash_type hash, Key_ const & key) {
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return iterator(er.first);
        return end();
    }

    /** \note not in std::unordered_map */
    template <typename Key_,
              SHAREMIND_REQUIRES_CONCEPTS(
                    Not(UnaryPredicate(Key_, key_type const &)))>
    const_iterator find(hash_type hash, Key_ const & key) const {
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return const_iterator(er.first);
        return end();
    }

    /** \note not in std::unordered_map */
    template <typename Pred,
              SHAREMIND_REQUIRES_CONCEPTS(
                    UnaryPredicate(Pred, key_type const &))>
    iterator find(hash_type hash, Pred pred) {
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (pred(er.first->second->first))
                return iterator(er.first);
        return end();
    }

    /** \note not in std::unordered_map */
    template <typename Pred,
              SHAREMIND_REQUIRES_CONCEPTS(
                    UnaryPredicate(Pred, key_type const &))>
    const_iterator find(hash_type hash, UnaryPredicate pred) const {
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (pred(er.first->second->first))
                return const_iterator(er.first);
        return end();
    }

    /** \note not in std::unordered_map */
    template <typename Pred,
              typename Key_,
              SHAREMIND_REQUIRES_CONCEPTS(
                    BinaryPredicate(Pred, key_type const &, Key_ const &))>
    iterator find(hash_type hash, Pred pred, Key_ const & key) {
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (pred(er.first->second->first, key))
                return iterator(er.first);
        return end();
    }

    /** \note not in std::unordered_map */
    template <typename Pred,
              typename Key_,
              SHAREMIND_REQUIRES_CONCEPTS(
                    BinaryPredicate(Pred, key_type const &, Key_ const &))>
    const_iterator find(hash_type hash, Pred pred, Key_ const & key)
            const
    {
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (pred(er.first->second->first, key))
                return const_iterator(er.first);
        return end();
    }

    size_type count(key_type const & key) const {
        size_type count = 0u;
        auto const hash(m_hasher(key));
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                ++count;
        return count;
    }

    /** \note not in std::unordered_map */
    template <typename Pred,
              SHAREMIND_REQUIRES_CONCEPTS(
                    UnaryPredicate(Pred, key_type const &))>
    size_type count(hash_type hash, Pred pred) const {
        size_type count = 0u;
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (pred(er.first->second->first))
                ++count;
        return count;
    }

    /** \note not in std::unordered_map */
    template <typename Pred,
              typename Key_,
              SHAREMIND_REQUIRES_CONCEPTS(
                    BinaryPredicate(Pred, key_type const &, Key_ const &))>
    size_type count(hash_type hash, Pred pred, Key_ const & key)
            const
    {
        size_type count = 0u;
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (pred(er.first->second->first, key))
                ++count;
        return count;
    }

    /** \note some not in std::unordered_map */
    template <typename ... Args>
    std::pair<iterator, iterator> equal_range(Args && ... args) {
        /* We rely on the fact the this is not a multimap: */
        std::pair<iterator, iterator> r(find(std::forward<Args>(args)...),
                                        end());
        if (r.first != r.second)
            r.second = std::next(r.first);
        return r;
    }

    /** \note some not in std::unordered_map */
    template <typename ... Args>
    std::pair<const_iterator, const_iterator> equal_range(Args && ... args)
            const
    {
        /* We rely on the fact the this is not a multimap: */
        std::pair<const_iterator, const_iterator> r(
                    find(std::forward<Args>(args)...),
                    end());
        if (r.first != r.second)
            r.second = std::next(r.first);
        return r;
    }

    mapped_type & operator[](key_type const & key) {
        auto const hash(m_hasher(key));
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return er.first->second->second;
        /** \todo optimize (hash calculated twice; hint) */
        return emplace(key, mapped_type()).first->second;
    }

    mapped_type & operator[](key_type && key) {
        auto const hash(m_hasher(key));
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return er.first->second->second;
        /** \todo optimize (hash calculated twice; hint) */
        return emplace(std::move(key), mapped_type()).first->second;
    }

    mapped_type & at(key_type const & key) {
        auto const hash(m_hasher(key));
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return er.first->second->second;
        throw std::out_of_range("No such element in UnorderedMap!");
    }

    mapped_type const & at(key_type const & key) const {
        auto const hash(m_hasher(key));
        for (auto er(m_container.equal_range(hash));
             er.first != er.second;
             ++er.first)
            if (m_pred(er.first->second->first, key))
                return er.first->second->second;
        throw std::out_of_range("No such element in UnorderedMap!");
    }


    /* Bucket interface: */

    size_type bucket_count() const noexcept
    { return m_container.bucket_count(); }

    size_type max_bucket_count() const noexcept
    { return m_container.max_bucket_count(); }

    size_type bucket_size(size_type n) const
    { return m_container.bucket_size(n); }

    size_type bucket(key_type const & key) const
    { return m_container.bucket(m_hasher(key)); }

    /** \note not in std::unordered_map */
    size_type hash_bucket(hash_type hash) const
    { return m_container.bucket(std::move(hash)); }

    /** \note not in std::unordered_map */
    size_type bucket(const_iterator it) const
    { return m_container.bucket(it.base()->first); }

    local_iterator begin(size_type n)
    { return local_iterator(m_container.begin(n)); }

    const_local_iterator begin(size_type n) const
    { return const_local_iterator(m_container.begin(n)); }

    local_iterator end(size_type n)
    { return local_iterator(m_container.end(n)); }

    const_local_iterator end(size_type n) const
    { return const_local_iterator(m_container.end(n)); }

    const_local_iterator cbegin(size_type n) const
    { return const_local_iterator(m_container.begin(n)); }

    const_local_iterator cend(size_type n) const
    { return const_local_iterator(m_container.end(n)); }


    /* Hash policy: */

    float load_factor() const noexcept { return m_container.load_factor(); }

    float max_load_factor() const noexcept
    { return m_container.max_load_factor(); }

    void max_load_factor(float z) { m_container.max_load_factor(z); }

    void rehash(size_type n) { m_container.rehash(n); }

    void reserve(size_type n) { m_container.reserve(n); }

private: /* Fields: */

    hasher m_hasher;
    key_equal m_pred;
    InnerContainer m_container;
    allocator_type m_allocator;

}; /* class UnorderedMap { */

template <typename Key, typename T, typename Hash, typename Pred, typename A>
bool operator==(UnorderedMap<Key, T, Hash, Pred, A> const & lhs,
                UnorderedMap<Key, T, Hash, Pred, A> const & rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    for (auto it = lhs.begin(); it != lhs.end();) {
        auto const ler(lhs.equal_range(it->first));
        auto const rer(rhs.equal_range(it->first));
        if (std::distance(ler.first, ler.second)
            != std::distance(rer.first, rer.second))
            return false;
        if (!std::is_permutation(ler.first, ler.second, rer.first))
            return false;
        it = ler.second;
    }
    return true;
}

template <typename Key, typename T, typename Hash, typename Pred, typename A>
bool operator!=(UnorderedMap<Key, T, Hash, Pred, A> const & lhs,
                UnorderedMap<Key, T, Hash, Pred, A> const & rhs)
{ return !(lhs == rhs); }

} /* namespace Sharemind { */

#endif /* SHAREMIND_UNORDEREDMAP_H */
