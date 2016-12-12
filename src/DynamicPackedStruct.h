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

#ifndef SHAREMIND_DYNAMICPACKEDSTRUCT_H
#define SHAREMIND_DYNAMICPACKEDSTRUCT_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>
#include "compiler-support/ClangPR26692.h"
#include "DynamicPackingInfo.h"
#include "GlobalDeleter.h"
#include "TemplateInstantiateWithTypeParams.h"
#include "TemplatePrefixTypes.h"
#include "TemplateSuffixTypes.h"


namespace sharemind {

template <typename ... Ts>
class DynamicPackedStruct {

public: /* Constants: */

    constexpr static std::size_t minSizeInBytes =
            DynamicPackingInfo<Ts...>::minSizeInBytes;

    constexpr static std::size_t maxSizeInBytes =
            DynamicPackingInfo<Ts...>::maxSizeInBytes;

    constexpr static std::size_t numFields =
            DynamicPackingInfo<Ts...>::numFields;

    constexpr static std::size_t numDynamicFields =
            DynamicPackingInfo<Ts...>::numDynamicFields;

    constexpr static std::size_t hasDynamicFields =
            DynamicPackingInfo<Ts...>::hasDynamicFields;

public: /* Types: */

    using type = DynamicPackedStruct<Ts...>;

    using DataPtr = std::unique_ptr<void, GlobalDeleter>;

    using AccumArray = typename DynamicPackingInfo<Ts...>::AccumArrayType;

    template <std::size_t I> using ElemType =
            typename DynamicPackingInfo<Ts...>::template ElemType<I>;

    template <std::size_t I>
    using FieldTraits =
            typename DynamicPackingInfo<Ts...>::template TypeTraits<I>;

    template <std::size_t I>
    using ReferenceType =
            typename DynamicPackingInfo<Ts...>::template ReferenceType<I>;

    template <std::size_t I>
    using ConstReferenceType =
            typename DynamicPackingInfo<Ts...>::template ConstReferenceType<I>;

    template <std::size_t I>
    using PointerType =
            typename DynamicPackingInfo<Ts...>::template PointerType<I>;

    template <std::size_t I>
    using ConstPointerType =
            typename DynamicPackingInfo<Ts...>::template ConstPointerType<I>;

    template <std::size_t I>
    using PrefixType =
            TemplateInstantiateWithTypeParams_t<
                SHAREMIND_CLANGPR26692_WORKAROUND(sharemind)
                    DynamicPackedStruct,
                TemplatePrefixTypes_t<I, Ts...>
            >;

    template <std::size_t I>
    using SuffixType =
            TemplateInstantiateWithTypeParams_t<
                SHAREMIND_CLANGPR26692_WORKAROUND(sharemind)
                    DynamicPackedStruct,
                TemplateSuffixTypes_t<I, Ts...>
            >;

    using StaticPrefixType =
            TemplateInstantiateWithTypeParams_t<
                SHAREMIND_CLANGPR26692_WORKAROUND(sharemind)
                    DynamicPackedStruct,
                typename DynamicPackingInfo<Ts...>::StaticPrefixType
            >;

public: /* Methods: */

    template <typename ... Args>
    DynamicPackedStruct(Args && ... args)
        : m_data(
              ( assert(DynamicPackingInfo<Ts...>::validSizes(args...)),
                ::operator new(
                  DynamicPackingInfo<Ts...>::sizeInBytes(
                      std::forward<Args>(args)...))))
        , m_sizes(std::forward<Args>(args)...)
    {}

    DynamicPackedStruct(type && move)
        : m_data(std::move(move.m_data))
        , m_sizes(std::move(move.m_sizes))
    {}

    DynamicPackedStruct(type & copy)
        : DynamicPackedStruct(static_cast<type const &>(copy))
    {}

    DynamicPackedStruct(type const & copy)
        : m_data(copy.m_data ? ::operator new(copy.size()) : nullptr)
        , m_sizes(copy.m_sizes)
    {
        if (m_data)
            std::memcpy(m_data.get(), copy.m_data.get(), copy.size());
    }

    DynamicPackedStruct & operator=(type && move) noexcept {
        m_data = std::move(move.m_data);
        m_sizes = std::move(move.m_sizes);
    }

    DynamicPackedStruct & operator=(type const & copy) {
        constexpr static auto lastSizesIndex = numDynamicFields - 1u;
        if (copy.m_data) {
            if (m_data) {
                if (m_sizes[lastSizesIndex] != copy.m_sizes[lastSizesIndex]) {
                    if (void * const newData =
                            ::operator new(copy.size(), std::nothrow))
                    {
                        m_data = newData;
                    } else {
                        if (m_sizes[lastSizesIndex]
                                < copy.m_sizes[lastSizesIndex])
                            throw std::bad_alloc();
                    }
                }
            } else {
                m_data = ::operator new(copy.size());
            }
            std::memcpy(m_data.get(), copy.m_data.get(), copy.size());
        } else {
            m_data.reset();
        }
        m_sizes = copy.m_sizes;
    }

    void * data() noexcept { return m_data.get(); }
    void const * data() const noexcept { return m_data.get(); }
    void const * constData() const noexcept { return m_data.get(); }

    DataPtr release() noexcept { return DataPtr(std::move(m_data)); }

    std::size_t size() const noexcept { return m_sizes.structSizeInBytes(); }

    template <std::size_t I> std::size_t elemOffset() const noexcept
    { return m_sizes.template elemOffset<I>(); }

    template <std::size_t I>
    void * voidPtr() const noexcept
    { return m_sizes.template voidPtr<I>(m_data.get()); }

    template <std::size_t I>
    void const * constVoidPtr() const noexcept
    { return m_sizes.template constVoidPtr<I>(m_data.get()); }

    void * endVoidPtr() noexcept
    { return m_sizes.endVoidPtr(m_data.get()); }

    void const * endVoidPtr() const noexcept
    { return m_sizes.endVoidPtr(m_data.get()); }

    void const * endConstVoidPtr() const noexcept
    { return m_sizes.endConstVoidPtr(m_data.get()); }

    template <std::size_t I>
    PointerType<I> ptr() const noexcept
    { return m_sizes.template ptr<I>(m_data.get()); }

    template <std::size_t I>
    ConstPointerType<I> constPtr() const noexcept
    { return m_sizes.template constPtr<I>(m_data.get()); }

    template <std::size_t I>
    ConstReferenceType<I> cref() const noexcept
    { return m_sizes.template cref<I>(m_data.get()); }

    template <std::size_t I>
    ReferenceType<I> ref() noexcept
    { return m_sizes.template ref<I>(m_data.get()); }

    template <std::size_t I>
    auto get() const noexcept ->
            decltype(std::declval<AccumArray const &>().template get<I>(
                         std::declval<void const *>()))
    { return m_sizes.template get<I>(m_data.get()); }

    template <std::size_t I>
    auto set(ElemType<I> const & v) noexcept ->
            decltype(std::declval<AccumArray const &>().template set<I>(
                         std::declval<void *>(), v))
    { return m_sizes.template set<I>(m_data.get(), v); }

    bool operator==(type const & rhs) const noexcept {
        auto const s = size();
        if (s != rhs.size())
            return false;
        return std::memcmp(m_data.get(), rhs.m_data.get(), size()) == 0;
    }

    bool operator!=(type const & rhs) const noexcept {
        auto const s = size();
        if (s != rhs.size())
            return true;
        return std::memcmp(m_data.get(), rhs.m_data.get(), size()) != 0;
    }

private: /* Fields: */

    DataPtr m_data;
    AccumArray m_sizes;

}; /* class DynamicPackedStruct */

template <typename T> struct DynamicPackedStructFromTemplate {};

template <template <typename...> class Tmpl, typename ... Ts>
struct DynamicPackedStructFromTemplate<Tmpl<Ts...> >
{ using type = DynamicPackedStruct<Ts...>; };

template <typename T>
using DynamicPackedStructFromTemplate_t =
        typename DynamicPackedStructFromTemplate<T>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_DYNAMICPACKEDSTRUCT_H */
