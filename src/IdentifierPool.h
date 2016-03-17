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

#ifndef SHAREMIND_IDENTIFIERPOOL_H
#define SHAREMIND_IDENTIFIERPOOL_H

#include <boost/intrusive/avl_set.hpp>
#include <limits>
#include <stdexcept>
#include <set>
#include <type_traits>
#include "Exception.h"


namespace sharemind {

template <typename T>
class IdentifierPool {

    static_assert(std::is_unsigned<T>::value,  "T must be unsigned!");
    static_assert(!std::is_const<T>::value,    "T must not be const!");
    static_assert(!std::is_volatile<T>::value, "T must not be volatile!");

public: /* Types: */

    SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);
    /**
      \brief Signifies that ID reservation failed because the identifier space
             in type T is exhausted.
    */
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            ReserveException,
            "No more identifiers can be reserved.");

private: /* Types: */

    struct Item: public boost::intrusive::avl_set_base_hook<> {
        Item(Item const &) = delete;
        Item & operator=(Item const &) = delete;

        inline Item() noexcept {}
        inline Item(T const v) noexcept : value(v) {}
        inline Item(Item && move) noexcept
            : value(std::move(move.value))
        { swap_nodes(move); }

        Item & operator=(Item && move) noexcept {
            value = std::move(move.value);
            swap_nodes(move);
        }

        inline bool operator==(Item const & v) const noexcept
        { return value == v.value; }
        inline bool operator!=(Item const & v) const noexcept
        { return value != v.value; }
        inline bool operator>(Item const & v) const noexcept
        { return value > v.value; }
        inline bool operator<(Item const & v) const noexcept
        { return value < v.value; }

        inline bool operator==(T const v) const noexcept { return value == v; }
        inline bool operator!=(T const v) const noexcept { return value != v; }
        inline bool operator>(T const v) const noexcept { return value > v; }
        inline bool operator<(T const v) const noexcept { return value < v; }

        T value;
    };
    static_assert(std::is_nothrow_move_assignable<Item>::value, "");
    static_assert(std::is_nothrow_move_constructible<Item>::value, "");
    static_assert(!std::is_copy_assignable<Item>::value, "");
    static_assert(!std::is_copy_constructible<Item>::value, "");

    using IdContainer =
              boost::intrusive::avl_set<
                  Item,
                  boost::intrusive::link_mode<boost::intrusive::normal_link> >;

public: /* Methods: */

    inline ~IdentifierPool() noexcept {
        constexpr static auto const disposer =
                [](Item * const item) { delete item; };
        m_reserved.clear_and_dispose(disposer);
        m_recycled.clear_and_dispose(disposer);
    }

    /**
      \brief Generates, reserves and returns an unique ID from this pool.
      \returns the unique ID generated and reserved.
      \throws ReserveException on pool exhaustion.
      \throws std::bad_alloc when out of memory.
    */
    T reserve() {
        // Use recycled identifiers if possible:
        if (!m_recycled.empty()) { // do we have recycled IDs available; constant
            Item & item = *(m_recycled.begin()); // lowest recycled ID; constant
            m_recycled.erase(m_recycled.begin()); // Erase from old container; amortized constant
            m_reserved.insert(item); // Put into new container; logarithmic
            return item.value;
        }

        if (m_reserved.empty()) {
            Item * const item = new Item(0u);
            m_reserved.insert(*item); // constant
            return 0u;
        } else {
            T const value = (*(m_reserved.rbegin())).value; // constant
            if (value >= std::numeric_limits<T>::max())
                throw ReserveException();
            Item * const item = new Item(value + 1u);
            m_reserved.insert(m_reserved.end(), *item); // insert after last; amortized constant
            return value;
        }
    }

    /**
      \brief Returns whether the given ID is reserved.
      \param[in] id The ID to check.
      \returns whether the given ID is reserved.
    */
    inline bool reserved(T const id) const noexcept
    { return m_reserved.find(id) != m_reserved.end(); }

    /**
       \brief Releases the given ID back to the pool.
       \param[in] id The ID to release.
    */
    inline void recycle(T const id) noexcept {
        auto const it = m_reserved.find(id);
        if (it == m_reserved.end())
            return;
        Item & item = *it;
        m_reserved.erase(it); // worst O(log n), average O(1)
        m_recycled.insert(item); // O(log n)

        // Garbage collect all recycled items larger than max reserved ID:
        // O(n log n) on m_recycled.
        T const maxReserved = (m_reserved.empty()
                               ? 0u
                               : (*(m_reserved.rbegin())).value); // constant
        while (!m_recycled.empty()) { // constant
            auto const it = m_recycled.rbegin(); // constant
            Item * const item = &*it;
            if (item->value <= maxReserved) // constant
                break;
            m_recycled.erase(--(it.base())); // worst O(log n), average O(1)
            delete item;
        }
    }

private: /* Fields: */

    IdContainer m_reserved; ///< The ordered set of reserved IDs.
    IdContainer m_recycled; ///< The ordered set of released IDs.

};

} /* namespace sharemind { */

#endif /* SHAREMIND_IDENTIFIERPOOL_H */
