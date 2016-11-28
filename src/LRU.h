/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_MINER_LRU_H
#define SHAREMIND_MINER_LRU_H

#include <cassert>
#include <list>
#include <memory>
#include <sharemind/compiler-support/ClangVersion.h>
#include <sharemind/compiler-support/GccVersion.h>
#include <type_traits>
#include <unordered_map>


#if (defined(SHAREMIND_CLANG_VERSION) && SHAREMIND_CLANG_VERSION < 30900) \
    || (defined(SHAREMIND_GCC_VERSION) && SHAREMIND_GCC_VERSION < 40900)
#define SHAREMIND_LRU_LIST_WORKAROUND(...) __VA_ARGS__
#else
#define SHAREMIND_LRU_LIST_WORKAROUND(...) c ## __VA_ARGS__
#endif

namespace sharemind {

/** A simple Least Recently Used (LRU) cache. */
template<typename key_t, typename value_t>
class LRU {

private: /* Types: */

    using str_ptr = std::shared_ptr<value_t>;
    using weak_ptr = std::weak_ptr<value_t>;

    class CacheElement {

        static_assert(std::is_nothrow_move_constructible<key_t>::value, "");

    public: /* Methods: */

        inline CacheElement(key_t && key_, str_ptr && ptr) noexcept
            : m_key(std::move(key_))
            , m_strong_ptr(std::move(ptr))
        {}

        inline str_ptr getValue() noexcept
        { return m_strong_ptr ? m_strong_ptr : m_weak_ptr.lock(); }

        inline bool isStrong() const noexcept
        { return static_cast<bool>(m_strong_ptr); }

        inline bool expired() const noexcept { return m_weak_ptr.expired(); }

        inline void demote() noexcept {
            assert(m_strong_ptr);
            m_strong_ptr.reset();
        }

        inline void promote(str_ptr ptr) noexcept {
            assert(!m_strong_ptr);
            m_strong_ptr = std::move(ptr);
        }

        inline key_t const & key() const noexcept { return m_key; }

    private: /* Fields: */

        key_t const m_key;
        str_ptr m_strong_ptr;
        weak_ptr m_weak_ptr{m_strong_ptr};

    };

    using CacheList = std::list<CacheElement>;
    using CacheMap = std::unordered_map<key_t, typename CacheList::iterator>;

public: /* Methods: */

    inline LRU(std::size_t const limit)
            noexcept(std::is_nothrow_default_constructible<CacheList>::value
                     && std::is_nothrow_default_constructible<CacheMap>::value)
        : m_sizeLimit{(assert(limit > 0u), limit)}
    {}

    /** \brief Inserts a item into the cache. */
    inline void insert(key_t key, str_ptr value) {
        auto const it(m_cacheMap.find(key));
        // Insert new element:
        m_cacheList.emplace_front(std::move(key), std::move(value));
        if (it != m_cacheMap.end()) {
            auto & origin = it->second->isStrong() ? m_cacheList : m_weakList;
            // delete old element from list
            origin.erase(it->second);
            // update map
            it->second = m_cacheList.begin();
        } else {
            try {
                m_cacheMap[m_cacheList.front().key()] = m_cacheList.begin();
            } catch (...) {
                m_cacheList.pop_front();
                throw;
            }
        }
        grow();
    }

    template <typename Key>
    inline std::shared_ptr<value_t> get(Key && key) noexcept {
        auto const it(m_cacheMap.find(std::forward<Key>(key)));
        if (it == m_cacheMap.end())
            return nullptr;

        if (str_ptr ptr = it->second->getValue()) {
            // found an element
            if (it->second->isStrong()) {
                // Move found item to front of LRU list:
                m_cacheList.splice(
                            m_cacheList.SHAREMIND_LRU_LIST_WORKAROUND(begin)(),
                            m_cacheList,
                            it->second);
            } else {
                // keep a strong pointer
                it->second->promote(ptr);
                // move found item from weakList to cacheList
                m_cacheList.splice(
                            m_cacheList.SHAREMIND_LRU_LIST_WORKAROUND(begin)(),
                            m_weakList,
                            it->second);
                // make sure to throw out element from cacheList
                grow();
            }
            return ptr;
        } else {
            // did not get an element, therefore it must be a expired weak_ptr
            m_weakList.erase(it->second);
            m_cacheMap.erase(it);
        }

        return nullptr;
    }

    /** \brief Clears this LRU cache. */
    inline void clear() noexcept {
        m_cacheMap.clear();
        m_cacheList.clear();
        m_weakList.clear();
    }

private: /* Methods: */

    /** \brief Increases the size of cache or removes the least recently used
     * element */
    inline void grow() noexcept {
        // demote items that are over the limit
        while (m_cacheList.size() > m_sizeLimit) {
            // decrease ref count
            m_cacheList.back().demote();
            // move from cacheList to weakList
            m_weakList.splice(
                        m_weakList.SHAREMIND_LRU_LIST_WORKAROUND(begin)(),
                        m_cacheList,
                        --m_cacheList.SHAREMIND_LRU_LIST_WORKAROUND(end)());
        }

        // then do garbage collection
        for (auto it = m_weakList.SHAREMIND_LRU_LIST_WORKAROUND(begin)();
             it != m_weakList.SHAREMIND_LRU_LIST_WORKAROUND(end)();
             ++it)
        {
            if (it->expired()) {
                m_cacheMap.erase(it->key());
                m_weakList.erase(it);
            }
        }
    }

private: /* Fields */

    std::size_t const m_sizeLimit;
    CacheList m_cacheList;
    CacheList m_weakList;
    CacheMap m_cacheMap;

}; /* class LRU { */

} /* namespace sharemind { */

#undef SHAREMIND_LRU_LIST_WORKAROUND

#endif /* SHAREMIND_MINER_LRU_H */
