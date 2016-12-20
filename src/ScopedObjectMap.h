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

#ifndef SHAREMIND_SCOPEDOBJECTMAP_H
#define SHAREMIND_SCOPEDOBJECTMAP_H

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <sharemind/compiler-support/GccVersion.h>
#include <type_traits>
#include <utility>
#include "compiler-support/GccIsNothrowDestructible.h"
#include "compiler-support/GccPR44436.h"


namespace sharemind {

template <class Key,
          class T,
          class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<Key const, T *> > >
class ScopedObjectMap {

    static_assert(sharemind::is_nothrow_destructible<T>::value,
                  "T is required to be nothrow destructible");

    static_assert(sharemind::is_nothrow_destructible<T>::value,
                  "T is required to be nothrow destructible. Maybe T is an "
                  "incomplete type?");

private: /* Types: */

    using impl_t = std::map<Key, T *, Compare, Allocator>;
    #if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION >= 40800)
    static_assert(sharemind::is_nothrow_destructible<impl_t>::value,
                  "impl_t is required to be nothrow destructible");
    #endif

public: /* Types: */

    using key_type = typename impl_t::key_type;
    using mapped_type = typename impl_t::mapped_type;
    using value_type = typename impl_t::value_type;
    using iterator = typename impl_t::iterator;
    using const_iterator = typename impl_t::const_iterator;
    using size_type = typename impl_t::size_type;

public: /* Methods: */

    inline ScopedObjectMap() {}
    ScopedObjectMap(ScopedObjectMap &&) = default;
    ScopedObjectMap(ScopedObjectMap const &) = default;

    inline ~ScopedObjectMap() noexcept {
        for (value_type & valuePair : m_map)
            delete valuePair.second;
    }

    ScopedObjectMap & operator=(ScopedObjectMap &&) = default;
    ScopedObjectMap & operator=(ScopedObjectMap const &) = default;

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

    #if ! SHAREMIND_GCCPR44436
    template <typename ... Args>
    inline std::pair<iterator, bool> emplace(Args && ... args)
    { return m_map.emplace(std::forward<Args>(args)...); }
    #endif

    inline std::pair<iterator, bool> insert(value_type const & value)
    { return m_map.insert(value); }

    inline std::pair<iterator, bool> insert(key_type const & key,
                                            mapped_type const value)
    { return m_map.insert(value_type(key, value)); }

    template <typename ... Args>
    inline std::pair<iterator, bool> constructAndInsert(
            std::string const & name,
            Args && ... args)
    {
        T const * const value = new T(std::forward<Args>(args)...);
        try {
            return insert(name, value);
        } catch (...) {
            delete value;
            throw;
        }
    }

    inline void replaceOrInsert(key_type const & key, mapped_type const value) {
        std::pair<iterator, bool> const r =
                m_map.insert(value_type(key, value));
        if (!r.second) {
            mapped_type const old = r.first->second;
            r.first->second = value;
            delete old;
        }
    }

    /// \todo Make noexcept:
    inline void erase(iterator const pos)
            noexcept(noexcept(std::declval<impl_t &>()
                                  .erase(std::declval<iterator const &>())))
    {
        delete (*pos).second;
        m_map.erase(pos);
    }

    /// \todo Make noexcept:
    inline void erase(iterator const first, iterator const last)
        noexcept(noexcept(std::declval<impl_t &>()
                              .erase(std::declval<iterator const &>(),
                                     std::declval<iterator const &>())))
    {
        for (iterator it(first); it != last; ++it)
            delete (*it).second;
        m_map.erase(first, last);
    }

    /// \todo Make noexcept:
    inline size_type erase(key_type const & key)
            noexcept(noexcept(std::declval<impl_t &>()
                              .find(std::declval<key_type const &>()))
                     && noexcept(std::declval<impl_t &>()
                                 .erase(std::declval<iterator const &>())))
    {
        iterator const it = m_map.find(key);
        if (it == m_map.end())
            return 0u;

        delete (*it).second;
        m_map.erase(it);
        return 1u;
    }

    /// \todo Make noexcept:
    inline void eraseNoDelete(iterator const pos)
            noexcept(noexcept(std::declval<impl_t &>()
                                  .erase(std::declval<iterator const &>())))
    { m_map.erase(pos); }

    /// \todo Make noexcept:
    inline void eraseNoDelete(iterator const first, iterator const last)
            noexcept(noexcept(std::declval<impl_t &>()
                                  .erase(std::declval<iterator const &>(),
                                         std::declval<iterator const &>())))
    { m_map.erase(first, last); }

    /// \todo Make noexcept:
    inline size_type eraseNoDelete(key_type const & key)
            noexcept(noexcept(std::declval<impl_t &>().erase(
                                  std::declval<key_type const &>())))
    { return m_map.erase(key); }

    /**********
     * Lookup *
     **********/

    /// \todo Make noexcept:
    inline size_type count(key_type const & key)
            const noexcept(noexcept(std::declval<impl_t const &>().count(
                                       std::declval<key_type const &>())))
    { return m_map.count(key); }

    /// \todo Make noexcept:
    inline iterator find(key_type const & key)
            noexcept(noexcept(std::declval<impl_t &>().find(
                                 std::declval<key_type const &>())))
    { return m_map.find(key); }

    /// \todo Make noexcept:
    inline const_iterator find(key_type const & key)
            const noexcept(noexcept(std::declval<impl_t const &>().find(
                                       std::declval<key_type const &>())))
    { return m_map.find(key); }

    /// \todo Make noexcept:
    inline mapped_type maybeAt(key_type const & key)
            const noexcept(noexcept(std::declval<impl_t const &>().find(
                                       std::declval<key_type const &>())))
    {
        const_iterator const it = m_map.find(key);
        return it == m_map.end() ? nullptr : it->second;
    }

    /// \todo Make noexcept:
    inline T & at(key_type const & key)
            const noexcept(noexcept(*std::declval<impl_t const &>().at(key)))
    { return *m_map.at(key); }

private: /* Fields: */

    impl_t m_map;

}; /* class ScopedObjectMap */

} /* namespace sharemind { */

#endif /* SHAREMIND_SCOPEDOBJECTMAP_H */
