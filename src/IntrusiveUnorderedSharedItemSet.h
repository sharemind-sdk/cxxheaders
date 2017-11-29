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
namespace Detail {
namespace IntrusiveUnorderedSharedItemSet {

template <typename, bool> struct ItemTraits;

} /* namespace IntrusiveUnorderedSharedItemSet { */
} /* namespace Detail { */

template <typename T, typename ... Args>
class IntrusiveUnorderedSharedItemSet;

template <typename T>
class IntrusiveUnorderedSharedItemSetItemBase
        : public boost::intrusive::unordered_set_base_hook<
                    boost::intrusive::link_mode<boost::intrusive::normal_link>,
                    boost::intrusive::store_hash<true> >
{

    template <typename, bool>
    friend struct Detail::IntrusiveUnorderedSharedItemSet::ItemTraits;

public: /* Methods: */

    virtual ~IntrusiveUnorderedSharedItemSetItemBase() noexcept {}

private: /* Fields: */

    mutable std::shared_ptr<T> m_selfPtr;

};

namespace Detail {
namespace IntrusiveUnorderedSharedItemSet {


template <typename T>
struct IsValidBase {

    template <typename V>
    static decltype(
            static_cast<IntrusiveUnorderedSharedItemSetItemBase<V> const &>(
                std::declval<T>()),
            std::true_type())
    test(IntrusiveUnorderedSharedItemSetItemBase<V> const &);

    static std::false_type test(...);

    constexpr static bool const value =
            decltype(test(std::declval<T>()))::value;
};

template <typename T, bool = IsValidBase<T>::value>
struct ItemTraits { constexpr static bool const isValid = false; };

template <typename T>
struct ItemTraits<T, true> {

    constexpr static bool const isValid = true;

    template <typename V>
    static decltype(
            static_cast<IntrusiveUnorderedSharedItemSetItemBase<V> const &>(
                std::declval<T>()),
            IntrusiveUnorderedSharedItemSetItemBase<V>())
    holderTypeTest(IntrusiveUnorderedSharedItemSetItemBase<V> const &);

    using HolderType = decltype(holderTypeTest(std::declval<T>()));

    static std::shared_ptr<T> const & getPtr(HolderType const & holder) noexcept
    { return holder.m_selfPtr; }

    static std::shared_ptr<T> takePtr(HolderType const & holder) noexcept
    { return std::shared_ptr<T>(std::move(holder.m_selfPtr)); }

    static void putPtr(HolderType & holder, std::shared_ptr<T> ptr) noexcept
    { holder.m_selfPtr = std::move(ptr); }

};

} /* namespace IntrusiveUnorderedSharedItemSet { */
} /* namespace Detail { */

template <typename T, typename ... Args>
class IntrusiveUnorderedSharedItemSet {

public: /* Constants: */

    constexpr static std::size_t defaultNumberOfBuckets() { return 200u; }

private: /* Types: */

    using ItemTraits = Detail::IntrusiveUnorderedSharedItemSet::ItemTraits<T>;
    using IsValidBase = Detail::IntrusiveUnorderedSharedItemSet::IsValidBase<T>;
    static_assert(IsValidBase::value, "Invalid item type!");

    using Container = boost::intrusive::unordered_set<T, Args...>;

public: /* Types: */

    using ValueType = typename Container::value_type;
    using SizeType = typename Container::size_type;
    using ConstIterator = typename Container::const_iterator;
    using Iterator = typename Container::iterator;
    using ConstReference = typename Container::const_reference;
    using Reference = typename Container::reference;

public: /* Methods: */

    inline IntrusiveUnorderedSharedItemSet(
            std::size_t const numBuckets = defaultNumberOfBuckets())
        : m_numBuckets(numBuckets)
    {}

    inline ~IntrusiveUnorderedSharedItemSet() noexcept {
        m_container.clear_and_dispose(
                    &IntrusiveUnorderedSharedItemSet::disposer);
    }

    Iterator begin() noexcept { return m_container.begin(); }
    ConstIterator begin() const noexcept { return m_container.begin(); }
    ConstIterator cbegin() const noexcept { return m_container.cbegin(); }

    Iterator end() noexcept { return m_container.end(); }
    ConstIterator end() const noexcept { return m_container.end(); }
    ConstIterator cend() const noexcept { return m_container.cend(); }

    bool empty() const noexcept { return m_container.empty(); }

    SizeType size() const noexcept { return m_container.size(); }

    Iterator iteratorTo(Reference value) noexcept
    { return m_container.iterator_to(value); }

    ConstIterator iteratorTo(ConstReference value) const noexcept
    { return m_container.iterator_to(value); }

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
        auto & item = *v;
        auto r(m_container.insert(item));
        ItemTraits::putPtr(item, std::move(v));
        return r;
    }

    void erase(ConstIterator it)
            noexcept(noexcept(std::declval<Container &>().erase_and_dispose(
                                  std::move(it),
                                  &IntrusiveUnorderedSharedItemSet::disposer)))
    {
        return m_container.erase_and_dispose(
                    std::move(it),
                    &IntrusiveUnorderedSharedItemSet::disposer);
    }

    SizeType erase(ConstReference cRef)
            noexcept(noexcept(std::declval<Container &>().erase_and_dispose(
                                  cRef,
                                  &IntrusiveUnorderedSharedItemSet::disposer)))
    {
        return m_container.erase_and_dispose(
                    cRef,
                    &IntrusiveUnorderedSharedItemSet::disposer);
    }

    std::shared_ptr<T> take(ConstIterator it)
            noexcept(noexcept(std::declval<Container &>().erase(std::move(it))))
    {
        auto r(ItemTraits::takePtr(*it));
        m_container.erase(std::move(it));
        return r;
    }

    std::shared_ptr<T> take(Reference ref)
            noexcept(noexcept(std::declval<Container &>().erase(ref)))
    {
        auto r(ItemTraits::takePtr(ref));
        m_container.erase(ref);
        return r;
    }

    static inline std::shared_ptr<ValueType> retrieveSharedPtr(
            ConstIterator const it) noexcept
    { return ItemTraits::getPtr(*it); }

    static inline std::shared_ptr<ValueType> retrieveSharedPtr(
            ConstReference const cref) noexcept
    { return ItemTraits::getPtr(cref); }

private: /* Methods: */

    static void disposer(T * const item) noexcept
    { ItemTraits::takePtr(*item); }

private: /* Fields: */

    std::size_t const m_numBuckets;
    std::unique_ptr<typename Container::bucket_type[]> const m_buckets{
        makeUnique<typename Container::bucket_type[]>(m_numBuckets)};
    Container m_container{
        typename Container::bucket_traits(m_buckets.get(), m_numBuckets)};

}; /* class IntrusiveUnorderedSharedItemSet */

} /* namespace sharemind { */

#endif /* SHAREMIND_INTRUSIVEUNORDEREDSHAREDITEMSET_H */
