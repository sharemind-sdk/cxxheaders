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

template <typename K, typename T>
class SharedResourceMap {

private: /* Types: */

    struct DefaultConstructor_ { T * operator()() const { return new T; } };

    struct ValueObj_ {
        std::weak_ptr<T> weakPtr;
        std::unique_ptr<T> realPtr;
    };

    using Map_ = std::map<K, ValueObj_>;

public: /* Types: */

    using type = SharedResourceMap<K, T>;

public: /* Methods: */

    template <typename F>
    void forEach(F f) const {
        std::unique_lock<std::mutex> lock(m_mutex);
        for (auto const & v : m_data)
            if (auto ptr = v.second.weakPtr.lock())
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

    template <typename Constructor = DefaultConstructor_>
    std::shared_ptr<T> getResource(K const & key,
                                   Constructor c = Constructor())
    {
        static auto const createShared = [](type & sharedResourceMap,
                                            K const & key,
                                            ValueObj_ & valueObj)
        {
            std::shared_ptr<T> r(
                valueObj.realPtr.get(),
                [&sharedResourceMap, key](T * const) noexcept {
                    std::lock_guard<std::mutex> const guard(
                                sharedResourceMap.m_mutex);
                    auto const it = sharedResourceMap.m_data.find(key);
                    if (it != sharedResourceMap.m_data.end()) {
                        ValueObj_ & obj = it->second;
                        if (obj.weakPtr.expired()) {
                            sharedResourceMap.m_data.erase(it);
                            sharedResourceMap.m_cond.notify_all();
                        }
                    }
                });
            valueObj.weakPtr = r;
            return r;
        };

        std::lock_guard<std::mutex> const guard(m_mutex);
        auto it = m_data.find(key);
        if (it != m_data.end()) {
            ValueObj_ & obj = it->second;
            if (auto s = obj.weakPtr.lock())
                return s;
            return createShared(*this, key, obj);
        } else {
            using VT = typename Map_::value_type;
            auto const rp =
                    m_data.SHAREMIND_GCCPR44436_METHOD(VT(key, ValueObj_()));
            try {
                assert(rp.second);
                ValueObj_ & obj = rp.first->second;
                obj.realPtr.reset(c());
                m_cond.notify_all();
                return createShared(*this, key, obj);
            } catch (...) {
                m_data.erase(rp.first);
                throw;
            }
        }
    }

private: /* Fields: */

    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cond;
    Map_ m_data;

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_MIN_H */
