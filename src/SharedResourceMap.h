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

#ifndef SHAREMIND_SHAREDRESOURCEMAP_H
#define SHAREMIND_SHAREDRESOURCEMAP_H

#include <cassert>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include "compiler-support/GccPR44436.h"


namespace sharemind {

template <typename Key, typename Value>
class SharedResourceMap {

private: /* Types: */

    struct DefaultConstructor_
    { Value * operator()(Key const &) const { return new Value; } };

    struct InnerBase {
        virtual ~InnerBase() noexcept {}
    };

    struct ValueObj_ {

    /* Methods: */

        template <typename C,
                  typename K,
                  typename Constructor,
                  typename ... Args>
        ValueObj_(C && c, K && k, Constructor constructor, Args && ... args)
                noexcept
            : container(std::forward<C>(c))
            , key(std::forward<K>(k))
            , realPtr(constructor(key, std::forward<Args>(args)...))
        {}

    /* Fields: */

        std::weak_ptr<InnerBase> container;
        std::weak_ptr<Value> weakPtr;
        Key key;
        #if SHAREMIND_GCCPR44436
        std::shared_ptr<Value>
        #else
        std::unique_ptr<Value>
        #endif
                realPtr;

    };

    using Map_ = std::map<Key, std::shared_ptr<ValueObj_> >;

    struct Inner final: InnerBase {

    /* Methods: */

        template <typename F>
        void forEach(F f) const {
            std::unique_lock<std::mutex> lock(m_mutex);
            for (auto const & v : m_data)
                if (auto ptr = v.second->weakPtr.lock())
                    f(v.first, std::move(ptr));
        }

        void waitForEmpty() const noexcept {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock, [this]() noexcept { return m_data.empty(); });
        }

        auto empty() const noexcept(noexcept(std::declval<Map_ const &>().empty()))
                -> decltype(std::declval<Map_ const &>().empty())
        {
            std::lock_guard<std::mutex> const guard(m_mutex);
            return m_data.empty();
        }

        auto size() const noexcept(noexcept(std::declval<Map_ const &>().size()))
                -> decltype(std::declval<Map_ const &>().size())
        {
            std::lock_guard<std::mutex> const guard(m_mutex);
            return m_data.size();
        }

    /* Fields: */

        mutable std::mutex m_mutex;
        mutable std::condition_variable m_cond;
        Map_ m_data;
    };

public: /* Types: */

    using type = SharedResourceMap<Key, Value>;

public: /* Methods: */

    virtual ~SharedResourceMap() noexcept {}

    template <typename F>
    auto forEach(F && f) const
            noexcept(noexcept(
                         std::declval<Inner const *>()->forEach(
                             std::forward<F>(f))))
            -> decltype(std::declval<Inner const *>()->forEach(
                            std::forward<F>(f)))
    { return m_inner->forEach(std::forward<F>(f)); }

    auto waitForEmpty() const
            noexcept(noexcept(std::declval<Inner const *>()->waitForEmpty()))
            -> decltype(std::declval<Inner const *>()->waitForEmpty())
    { return m_inner->waitForEmpty(); }

    auto empty() const
            noexcept(noexcept(std::declval<Inner const *>()->empty()))
            -> decltype(std::declval<Inner const *>()->empty())
    { return m_inner->empty(); }

    auto size() const
            noexcept(noexcept(std::declval<Inner const *>()->size()))
            -> decltype(std::declval<Inner const *>()->size())
    { return m_inner->size(); }

    template <typename K>
    std::shared_ptr<Value> getResource(K && key)
    { return getResource(std::forward<K>(key), DefaultConstructor_()); }

    template <typename K,
              typename Constructor,
              typename ... Args>
    std::shared_ptr<Value> getResource(
            K && key,
            Constructor && constructor,
            Args && ... args)
    {
        static auto const createShared = [](std::shared_ptr<ValueObj_> valueObj)
        {
            std::shared_ptr<Value> r(
                valueObj->realPtr.get(),
                [valueObj](Value * const) mutable noexcept {
                    if (auto cPtr = valueObj->container.lock()) {
                        Inner & c = *static_cast<Inner *>(cPtr.get());
                        std::lock_guard<std::mutex> const guard(c.m_mutex);
                        auto const it = c.m_data.find(valueObj->key);
                        if ((it != c.m_data.end())
                            && it->second->weakPtr.expired())
                        {
                            valueObj.reset();
                            c.m_data.erase(it);
                            c.m_cond.notify_all();
                        }
                    }
                });
            valueObj->weakPtr = r;
            return r;
        };

        std::lock_guard<std::mutex> const guard(m_inner->m_mutex);
        auto it = m_inner->m_data.find(key);
        if (it != m_inner->m_data.end()) {
            auto & obj = it->second;
            if (auto s = obj->weakPtr.lock())
                return s;
            return createShared(obj);
        } else {
            auto const rp =
                    m_inner->m_data.SHAREMIND_GCCPR44436_METHOD(
                        typename Map_::value_type{key, {}});
            assert(rp.second);
            try {
                auto & obj = rp.first->second;
                obj = std::make_shared<ValueObj_>(
                            m_inner,
                            std::forward<K>(key),
                            std::forward<Constructor>(constructor),
                            std::forward<Args>(args)...);
                m_inner->m_cond.notify_all();
                return createShared(obj);
            } catch (...) {
                m_inner->m_data.erase(rp.first);
                throw;
            }
        }
    }

private: /* Fields: */

    std::shared_ptr<Inner> m_inner{std::make_shared<Inner>()};

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_MIN_H */
