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

#ifndef SHAREMIND_IDENTIFIERPOOL_H
#define SHAREMIND_IDENTIFIERPOOL_H

#include <cassert>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <set>
#include <type_traits>
#include "detail/ExceptionMacros.h"
#include "Exception.h"
#include "StrongType.h"


namespace sharemind {

template <typename T>
class IdentifierPool {

    static_assert(std::is_unsigned<T>::value,  "T must be unsigned!");
    static_assert(!std::is_const<T>::value,    "T must not be const!");
    static_assert(!std::is_volatile<T>::value, "T must not be volatile!");

public: /* Types: */

    using ValueType = T;

    SHAREMIND_DETAIL_DEFINE_EXCEPTION(sharemind::Exception, Exception);
    /**
      \brief Signifies that ID reservation failed because the identifier space
             in type T is exhausted.
    */
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            ReserveException,
            "No more identifiers can be reserved.");

private: /* Types: */

    struct State {

    /* Methods: */

        inline T reserve() {
            std::lock_guard<std::mutex> const guard(m_mutex);
            T tryNext = m_tryNextId;
            T const oldTryNext = tryNext;
            while (m_reserved.find(tryNext) != m_reserved.end())
                if (++tryNext == oldTryNext)
                    throw ReserveException();
            m_reserved.insert(tryNext);
            m_tryNextId = tryNext + 1u;
            return tryNext;
        }

        /**
           \brief Releases the given ID back to the pool.
           \param[in] id The ID to release.
           \pre The given id is reserved by the pool.
           \post The given id is not reserved by the pool.
        */
        inline void recycle(T const id) noexcept {
            std::lock_guard<std::mutex> const guard(m_mutex);
            #ifndef NDEBUG
            auto const numErased =
            #endif
                    m_reserved.erase(id);
            assert(numErased > 0u);
        }

    /* Fields: */
        std::mutex m_mutex;
        std::set<T> m_reserved; /**< The ordered set of reserved IDs. */
        T m_tryNextId = 0u;     /**< The next ID to try to reserve. */
    };

public: /* Types: */

    class IdHolder {

        friend class IdentifierPool<T>;

    public: /* Types: */

        using ValueType = T;

    public: /* Methods: */

        IdHolder() {}

        IdHolder(IdHolder const & copy) = delete;
        IdHolder & operator=(IdHolder const & copy) = delete;

        IdHolder(IdHolder && move) noexcept
            : m_id(move.m_id)
            , m_state(std::move(move.m_state))
        {}

        IdHolder & operator=(IdHolder && move) noexcept {
            if (m_state)
                m_state->recycle(m_id);
            m_id = move.m_id;
            m_state = std::move(move.m_state);
            return *this;
        }

        ~IdHolder() noexcept {
            if (m_state)
                m_state->recycle(m_id);
        }

        operator bool() const noexcept { return valid(); }
        bool valid() const noexcept { return m_state != nullptr; }
        T id() const noexcept { return m_id; }
        T const & idRef() const noexcept { return m_id; }

        void release() noexcept {
            if (m_state) {
                m_state->recycle(m_id);
                m_state.reset();
            }
        }

    private: /* Methods: */

        IdHolder(std::shared_ptr<State> state) noexcept
            : m_id(state->reserve())
            , m_state(std::move(state))
        {}

    private: /* Fields: */

        T m_id;
        std::shared_ptr<State> m_state = nullptr;

    };

public: /* Methods: */

    /**
      \brief Generates, reserves and returns an unique ID from this pool.
      \post the returned ID is reserved by the pool.
      \returns the unique ID generated and reserved.
      \throws ReserveException on pool exhaustion.
      \throws std::bad_alloc when out of memory.
    */
    IdHolder reserve() { return m_state; }

private: /* Fields: */

    std::shared_ptr<State> m_state{std::make_shared<State>()};

};


template <typename T, typename Tag, typename ... Mixins>
class IdentifierPool<StrongType<T, Tag, Mixins...> > {

public: /* Types: */

    using ValueType = StrongType<T, Tag, Mixins...>;

    SHAREMIND_DETAIL_DEFINE_EXCEPTION(sharemind::Exception, Exception);
    /**
      \brief Signifies that ID reservation failed because the identifier space
             in type T is exhausted.
    */
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            ReserveException,
            "No more identifiers can be reserved.");

public: /* Types: */

    class IdHolder {

    public: /* Types: */

        using ValueType = StrongType<T, Tag, Mixins...>;

    public: /* Methods: */

        IdHolder() {}

        IdHolder(typename IdentifierPool<T>::IdHolder inner) noexcept
            : m_inner(std::move(inner))
        {}

        IdHolder(IdHolder const & copy) = delete;
        IdHolder & operator=(IdHolder const & copy) = delete;

        IdHolder(IdHolder && move) noexcept
            : m_inner(std::move(move.m_inner))
        {}

        IdHolder & operator=(IdHolder && move) noexcept {
            m_inner = std::move(move.m_inner);
            return *this;
        }

        ~IdHolder() noexcept = default;

        operator bool() const noexcept { return m_inner.valid(); }
        bool valid() const noexcept { return m_inner.valid(); }
        ValueType id() const noexcept { return ValueType(m_inner.id()); }

        void release() noexcept { m_inner.release(); }

    private: /* Fields: */

        typename IdentifierPool<T>::IdHolder m_inner;

    };

public: /* Methods: */

    /**
      \brief Generates, reserves and returns an unique ID from this pool.
      \post the returned ID is reserved by the pool.
      \returns the unique ID generated and reserved.
      \throws ReserveException on pool exhaustion.
      \throws std::bad_alloc when out of memory.
    */
    IdHolder reserve() { return m_inner.reserve(); }

private: /* Fields: */

    IdentifierPool<T> m_inner;

};

template <typename T>
using IdHolder = typename IdentifierPool<T>::IdHolder;

} /* namespace sharemind { */

#endif /* SHAREMIND_IDENTIFIERPOOL_H */
