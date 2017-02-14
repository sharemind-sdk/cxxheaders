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

#ifndef SHAREMIND_INTRUSIVEUNORDEREDSHAREDITEMSET_H
#define SHAREMIND_INTRUSIVEUNORDEREDSHAREDITEMSET_H

#include <cstddef>
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <memory>
#include <type_traits>
#include <utility>
#include "MakeUnique.h"


namespace sharemind {

template <typename T, typename ... Args>
class IntrusiveUnorderedSharedItemSet;

class IntrusiveUnorderedSharedItemSetItemBase
        : public boost::intrusive::unordered_set_base_hook<
                    boost::intrusive::link_mode<boost::intrusive::normal_link> >
{

    template <typename, typename ...>
    friend class IntrusiveUnorderedSharedItemSet;

public: /* Methods: */

    virtual ~IntrusiveUnorderedSharedItemSetItemBase() noexcept {}

private: /* Fields: */

    std::shared_ptr<IntrusiveUnorderedSharedItemSetItemBase> m_selfPtr;

};

template <typename T, typename ... Args>
class IntrusiveUnorderedSharedItemSet {

    static_assert(
            std::is_base_of<IntrusiveUnorderedSharedItemSetItemBase, T>::value,
            "T must inherit from IntrusiveUnorderedSharedItemSetItemBase!");

public: /* Constants: */

    constexpr static std::size_t defaultNumberOfBuckets = 200u;

private: /* Types: */

    using Container = boost::intrusive::unordered_set<T, Args...>;

public: /* Types: */

    using ConstIterator = typename Container::const_iterator;
    using Iterator = typename Container::iterator;

public: /* Methods: */

    inline IntrusiveUnorderedSharedItemSet(
            std::size_t const numBuckets = defaultNumberOfBuckets)
        : m_numBuckets(numBuckets)
    {}

    inline ~IntrusiveUnorderedSharedItemSet() noexcept {
        m_container.clear_and_dispose(
                    &IntrusiveUnorderedSharedItemSet::disposer);
    }

    template <typename KeyType, typename KeyHasher, typename KeyComparator>
    T * find(KeyType const & key,
             KeyHasher keyHasher,
             KeyComparator keyComparator)
    {
        auto const r = m_container.find(key, keyHasher, keyComparator);
        return (r != m_container.cend())? &*r : nullptr;
    }

    template <typename KeyType, typename KeyHasher, typename KeyComparator>
    T const * find(KeyType const & key,
                   KeyHasher keyHasher,
                   KeyComparator keyComparator) const
    {
        auto const r = m_container.find(key, keyHasher, keyComparator);
        return (r != m_container.cend())? &*r : nullptr;
    }

    std::pair<Iterator, bool> insert(std::shared_ptr<T> v)
        noexcept(noexcept(std::declval<Container &>().insert(*v)))
    {
        auto r(m_container.insert(*v));
        v->m_selfPtr = std::move(v);
        return r;
    }

    void erase(ConstIterator it)
            noexcept(noexcept(std::declval<Container &>().erase(std::move(it))))
    { return m_container.erase(std::move(it)); }

private: /* Methods: */

    static void disposer(IntrusiveUnorderedSharedItemSetItemBase * const item)
            noexcept
    { item->m_selfPtr.reset(); }

private: /* Fields: */

    std::size_t const m_numBuckets;
    std::unique_ptr<typename Container::bucket_type[]> const m_buckets{
        makeUnique<typename Container::bucket_type[]>(m_numBuckets)};
    Container m_container{
        typename Container::bucket_traits(m_buckets.get(), m_numBuckets)};

}; /* class IntrusiveUnorderedSharedItemSet */

} /* namespace sharemind { */

#endif /* SHAREMIND_INTRUSIVEUNORDEREDSHAREDITEMSET_H */
