/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_COMMON_SCOPEDOBJECTMAP_H
#define SHAREMIND_COMMON_SCOPEDOBJECTMAP_H

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <sharemind/compiler-support/GccIsNothrowDestructible.h>
#include <type_traits>
#include <utility>


namespace sharemind {

template <class Key,
          class T,
          class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T *> > >
class ScopedObjectMap {

    static_assert(std::is_nothrow_destructible<T>::value,
                  "T is required to be nothrow destructible");

    static_assert(std::is_nothrow_destructible<T>::value,
                  "T is required to be nothrow destructible. Maybe T is an "
                  "incomplete type?");

private: /* Types: */

    typedef std::map<Key, T *, Compare, Allocator> impl_t;
    #if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION >= 40800)
    static_assert(std::is_nothrow_destructible<impl_t>::value,
                  "impl_t is required to be nothrow destructible");
    #endif

public: /* Types: */

    typedef typename impl_t::key_type key_type;
    typedef typename impl_t::mapped_type mapped_type;
    typedef typename impl_t::value_type value_type;
    typedef typename impl_t::iterator iterator;
    typedef typename impl_t::const_iterator const_iterator;
    typedef typename impl_t::size_type size_type;

public: /* Methods: */

    inline ~ScopedObjectMap() noexcept {
        for (value_type & valuePair : m_map)
            delete valuePair.second;
    }

    /*************
     * Iterators *
     *************/

    inline iterator begin() noexcept { return m_map.begin(); }
    inline iterator end() noexcept { return m_map.end(); }
    inline const_iterator cbegin() const noexcept { return m_map.begin(); }
    inline const_iterator cend() const noexcept { return m_map.end(); }
    inline const_iterator begin() const noexcept { return cbegin(); }
    inline const_iterator end() const noexcept { return cend(); }

    /************
     * Capacity *
     ************/

    inline size_type empty() const noexcept { return m_map.empty(); }
    inline size_type size() const noexcept { return m_map.size(); }

    /*************
     * Modifiers *
     *************/

    inline void clear() noexcept {
        for (value_type & valuePair : m_map)
            delete valuePair.second;
        m_map.clear();
    }

    inline void clearNoDelete() noexcept { m_map.clear(); }

    inline std::pair<iterator, bool> insert(const value_type & value)
    { return m_map.insert(value); }

    inline std::pair<iterator, bool> insert(const key_type & key,
                                            const mapped_type value)
    { return m_map.insert(value_type(key, value)); }

    inline void replaceOrInsert(const key_type & key, const mapped_type value) {
        const std::pair<iterator, bool> r = m_map.insert(value_type(key,
                                                                    value));
        if (!r.second) {
            const mapped_type old = r.first->second;
            r.first->second = value;
            delete old;
        }
    }

    /// \todo Make noexcept:
    inline void erase(const iterator pos)
            noexcept(noexcept(std::declval<impl_t &>()
                                  .erase(std::declval<const iterator &>())))
    {
        delete (*pos).second;
        m_map.erase(pos);
    }

    /// \todo Make noexcept:
    inline void erase(const iterator first, const iterator last)
        noexcept(noexcept(std::declval<impl_t &>()
                              .erase(std::declval<const iterator &>(),
                                     std::declval<const iterator &>())))
    {
        for (iterator it(first); it != last; ++it)
            delete (*it).second;
        m_map.erase(first, last);
    }

    /// \todo Make noexcept:
    inline size_type erase(const key_type & key)
            noexcept(noexcept(std::declval<impl_t &>()
                              .find(std::declval<const key_type &>()))
                     && noexcept(std::declval<impl_t &>()
                                 .erase(std::declval<const iterator &>())))
    {
        const iterator it = m_map.find(key);
        if (it == m_map.end())
            return 0u;

        delete (*it).second;
        m_map.erase(it);
        return 1u;
    }

    /// \todo Make noexcept:
    inline void eraseNoDelete(const iterator pos)
            noexcept(noexcept(std::declval<impl_t &>()
                                  .erase(std::declval<const iterator &>())))
    { m_map.erase(pos); }

    /// \todo Make noexcept:
    inline void eraseNoDelete(const iterator first, const iterator last)
            noexcept(noexcept(std::declval<impl_t &>()
                                  .erase(std::declval<const iterator &>(),
                                         std::declval<const iterator &>())))
    { m_map.erase(first, last); }

    /// \todo Make noexcept:
    inline size_type eraseNoDelete(const key_type & key)
            noexcept(noexcept(std::declval<impl_t &>().erase(
                                  std::declval<const key_type &>())))
    { return m_map.erase(key); }

    /**********
     * Lookup *
     **********/

    /// \todo Make noexcept:
    inline size_type count(const key_type & key)
            const noexcept(noexcept(std::declval<const impl_t &>().count(
                                       std::declval<const key_type &>())))
    { return m_map.count(key); }

    /// \todo Make noexcept:
    inline iterator find(const key_type & key)
            noexcept(noexcept(std::declval<impl_t &>().find(
                                 std::declval<const key_type &>())))
    { return m_map.find(key); }

    /// \todo Make noexcept:
    inline const_iterator find(const key_type & key)
            const noexcept(noexcept(std::declval<const impl_t &>().find(
                                       std::declval<const key_type &>())))
    { return m_map.find(key); }

    /// \todo Make noexcept:
    inline mapped_type maybeAt(const key_type & key)
            const noexcept(noexcept(std::declval<const impl_t &>().find(
                                       std::declval<const key_type &>())))
    {
        const const_iterator it = m_map.find(key);
        return it == m_map.end() ? nullptr : it->second;
    }

    /// \todo Make noexcept:
    inline T & at(const key_type & key)
            const noexcept(noexcept(*std::declval<const impl_t &>().at(key)))
    { return *m_map.at(key); }

private: /* Fields: */

    impl_t m_map;

}; /* class ScopedObjectMap */

} /* namespace sharemind { */

#endif /* SHAREMIND_COMMON_SCOPEDOBJECTMAP_H */
