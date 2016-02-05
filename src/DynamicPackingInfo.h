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

#ifndef SHAREMIND_DYNAMICPACKINGINFO_H
#define SHAREMIND_DYNAMICPACKINGINFO_H

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include "ConstUnalignedReference.h"
#include "SizeOfTypes.h"
#include "TemplateCopyTypeParams.h"
#include "TemplateGetTypeParam.h"
#include "TemplatePrefixTypes.h"
#include "TemplateTypeFilter.h"
#include "UnalignedPointer.h"
#include "UnalignedReference.h"


namespace sharemind {

struct DynamicFieldPlaceholder
{ using type = DynamicFieldPlaceholder; };

template <typename T>
struct DynamicVectorFieldPlaceholder {
    using type = DynamicVectorFieldPlaceholder<T>;
    using valueType = T;
};

namespace Detail {
namespace DynamicPacking {

template <typename T>
struct FieldTraits {
    using ReferenceType = UnalignedReference<T>;
    using ConstReferenceType = ConstUnalignedReference<T>;
    using PointerType = UnalignedPointer<T>;
    using ConstPointerType = UnalignedPointer<T const>;
    constexpr static bool const isStatic = true;
};

template <>
struct FieldTraits<DynamicFieldPlaceholder> {
    using PointerType = void *;
    using ConstPointerType = void const *;
    constexpr static bool const isStatic = false;
};

template <typename T>
struct FieldTraits<DynamicVectorFieldPlaceholder<T> > {
    using ReferenceType = UnalignedReference<T>;
    using ConstReferenceType = ConstUnalignedReference<T>;
    using PointerType = UnalignedPointer<T>;
    using ConstPointerType = UnalignedPointer<T const>;
    constexpr static bool const isStatic = false;
};

template <typename T>
using DynamicFieldPred =
        std::integral_constant<bool, !FieldTraits<T>::isStatic>;

template <typename ... Ts>
using DynamicFieldFilter = TemplateTypeFilter_t<DynamicFieldPred, Ts...>;

template <typename ... Ts> struct AccumSize;

template <>
struct AccumSize<>
{ constexpr static std::size_t value() noexcept { return 0u; } };

template <typename ... Ts>
struct AccumSize<DynamicFieldPlaceholder, Ts...> {
    template <typename ... Args>
    constexpr static std::size_t value(std::size_t const size,
                                       Args && ... args) noexcept
    { return size + AccumSize<Ts...>::value(std::forward<Args>(args)...); }
};

template <typename T, typename ... Ts>
struct AccumSize<DynamicVectorFieldPlaceholder<T>, Ts...> {
    template <typename ... Args>
    constexpr static std::size_t value(std::size_t const size,
                                       Args && ... args) noexcept
    {
        return sizeof(T) * size
               + AccumSize<Ts...>::value(std::forward<Args>(args)...);
    }
};

template <typename T, typename ... Ts>
struct AccumSize<T, Ts...> {
    template <typename ... Args>
    constexpr static std::size_t value(Args && ... args) noexcept
    { return sizeof(T) + AccumSize<Ts...>::value(std::forward<Args>(args)...); }
};

template <typename ... Ts>
struct StaticTailSize;

template <>
struct StaticTailSize<>: std::integral_constant<std::size_t, 0u> {};

template <typename T, typename ... Ts>
struct StaticTailSize<T, Ts...>
        : std::conditional<
            DynamicFieldFilter<Ts...>::size <= 0u,
            std::integral_constant<std::size_t, sizeOfTypes<Ts...>()>,
            StaticTailSize<Ts...>
        >::type
{};

template <typename ... Ts>
struct AccumVecPopulator;

template <>
struct AccumVecPopulator<> {
    static void populate(std::size_t const * const, std::size_t const) noexcept
    {}
};

template <typename ... Ts>
struct AccumVecPopulator<DynamicFieldPlaceholder, Ts...> {
    template <typename ... Args>
    static void populate(std::size_t * const ptr,
                         std::size_t accum,
                         std::size_t const size,
                         Args && ... args) noexcept
    {
        accum += size;
        (*ptr) = accum;
        AccumVecPopulator<Ts...>::populate(ptr + 1u,
                                           accum,
                                           std::forward<Args>(args)...);
    }
};

template <typename T, typename ... Ts>
struct AccumVecPopulator<DynamicVectorFieldPlaceholder<T>, Ts...> {
    template <typename ... Args>
    static void populate(std::size_t * const ptr,
                         std::size_t accum,
                         std::size_t const size,
                         Args && ... args) noexcept
    {
        accum += sizeof(T) * size;
        (*ptr) = accum;
        AccumVecPopulator<Ts...>::populate(ptr + 1u,
                                           accum,
                                           std::forward<Args>(args)...);
    }
};

template <typename T, typename ... Ts>
struct AccumVecPopulator<T, Ts...> {
    template <typename ... Args>
    static void populate(std::size_t * const ptr,
                         std::size_t const accum,
                         Args && ... args) noexcept
    {
        AccumVecPopulator<Ts...>::populate(ptr,
                                           accum + sizeof(T),
                                           std::forward<Args>(args)...);
    }
};

} /* namespace DynamicPacking { */
} /* namespace Detail { */


template <typename ... Ts>
struct DynamicPackingInfo {

/* Constants: */

    constexpr static std::size_t const numTypes = sizeof...(Ts);

    constexpr static auto const numDynamicTypes =
            Detail::DynamicPacking::DynamicFieldFilter<Ts...>::size;

    constexpr static bool const hasDynamicTypes = numDynamicTypes > 0u;

/* Types: */

    using type = DynamicPackingInfo<Ts...>;

    struct AccumArrayType;

    template <std::size_t I>
    using ElemType = typename TemplateGetTypeParam<I, Ts...>::type;

    template <std::size_t I>
    using TypeTraits =
            Detail::DynamicPacking::FieldTraits<ElemType<I> >;

    template <std::size_t I>
    using ReferenceType = typename TypeTraits<I>::ReferenceType;

    template <std::size_t I>
    using ConstReferenceType = typename TypeTraits<I>::ConstReferenceType;

    template <std::size_t I>
    using PointerType = typename TypeTraits<I>::PointerType;

    template <std::size_t I>
    using ConstPointerType = typename TypeTraits<I>::ConstPointerType;

/* Methods: */

    template <typename ... Args>
    constexpr static std::size_t sizeInBytes(Args && ... args) noexcept {
        return Detail::DynamicPacking::AccumSize<Ts...>::value(
                    std::forward<Args>(args)...);
    }

    template <typename ... Args>
    static void populateAccumArray(std::size_t * const accumSizes,
                                   Args && ... args) noexcept
    {
        static_assert(sizeof...(Args) == numDynamicTypes,
                      "The number of arguments must match the number of "
                      "dynamic fields.");
        using P = Detail::DynamicPacking::AccumVecPopulator<Ts...>;
        P::populate(accumSizes, 0u, std::forward<Args>(args)...);
    }

    template <typename AccumSizes>
    static constexpr std::size_t sizeFromAccum(AccumSizes const & accumSizes)
            noexcept
    {
        return Detail::DynamicPacking::StaticTailSize<Ts...>::value
               + accumSizes[numDynamicTypes - 1u];
    }

    template <std::size_t I, typename AccumSizes>
    constexpr static
    typename std::enable_if<
        (TemplateCopyTypeParams_t<
            TemplatePrefixTypes_t<I, Ts...>,
            Detail::DynamicPacking::DynamicFieldFilter
        >::size > 0u) ? true : false,
        std::size_t
    >::type
    elemOffset(AccumSizes const & accumSizes) noexcept
    {
        return accumSizes[
                TemplateCopyTypeParams_t<
                    TemplatePrefixTypes_t<I, Ts...>,
                    Detail::DynamicPacking::DynamicFieldFilter
                >::size - 1u]
               + TemplateCopyTypeParams_t<
                    TemplatePrefixTypes_t<I, Ts...>,
                    Detail::DynamicPacking::StaticTailSize
                >::value;
    }

    template <std::size_t I, typename AccumSizes>
    constexpr static
    typename std::enable_if<
        (TemplateCopyTypeParams_t<
            TemplatePrefixTypes_t<I, Ts...>,
            Detail::DynamicPacking::DynamicFieldFilter
        >::size > 0u) ? false : true,
        std::size_t
    >::type
    elemOffset(AccumSizes const &) noexcept {
        return TemplateCopyTypeParams_t<TemplatePrefixTypes_t<I, Ts...>,
                                        SizeOfTypes>::value;
    }

    template <std::size_t I, typename AccumSizes>
    constexpr static void * voidPtr(void * const data,
                                    AccumSizes const & accumSizes) noexcept
    { return ptrAdd(data, elemOffset<I>(accumSizes)); }

    template <std::size_t I, typename AccumSizes>
    constexpr static void const * constVoidPtr(void const * const data,
                                               AccumSizes const & accumSizes)
            noexcept
    { return ptrAdd(data, elemOffset<I>(accumSizes)); }

    template <std::size_t I, typename AccumSizes>
    constexpr static PointerType<I> ptr(void * const data,
                                        AccumSizes const & accumSizes) noexcept
    { return voidPtr<I>(data, accumSizes); }

    template <std::size_t I, typename AccumSizes>
    constexpr static ConstPointerType<I> constPtr(void const * const data,
                                                  AccumSizes const & accumSizes)
            noexcept
    { return constVoidPtr<I>(data, accumSizes); }

    template <std::size_t I, typename AccumSizes>
    constexpr static ConstReferenceType<I> cref(void const * const data,
                                                AccumSizes const & accumSizes)
            noexcept
    { return constVoidPtr<I>(data, accumSizes); }

    template <std::size_t I, typename AccumSizes>
    constexpr static ReferenceType<I> ref(void * const data,
                                          AccumSizes const & accumSizes)
            noexcept
    { return voidPtr<I>(data, accumSizes); }

    template <std::size_t I, typename AccumSizes>
    constexpr static auto get(void const * const data,
                              AccumSizes const & accumSizes) noexcept ->
            typename std::enable_if<
                TypeTraits<I>::isStatic,
                ElemType<I>
            >::type
    { return cref<I>(data, accumSizes); }

    template <std::size_t I, typename AccumSizes>
    constexpr static auto set(void * const data,
                              AccumSizes const & accumSizes,
                              ElemType<I> const & v) noexcept ->
            typename std::enable_if<
                TypeTraits<I>::isStatic,
                decltype(ref<I>(data, accumSizes) = v)
            >::type
    { return ref<I>(data, accumSizes) = v; }

};

template <typename ... Ts>
struct DynamicPackingInfo<Ts...>::AccumArrayType
        : std::array<std::size_t, numDynamicTypes>
{

/* Types: */

    using type = AccumArrayType;

    using base = std::array<std::size_t, numDynamicTypes>;

    template <std::size_t I> using ElemType =
            typename DynamicPackingInfo<Ts...>::template ElemType<I>;

    template <std::size_t I>
    using TypeTraits =
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

/* Methods: */

    template <typename ... Args>
    AccumArrayType(Args && ... args) noexcept {
        DynamicPackingInfo<Ts...>::populateAccumArray(
                    this->data(),
                    std::forward<Args>(args)...);
    }

    AccumArrayType(type & copy) noexcept
        : AccumArrayType(const_cast<type const &>(copy))
    {}

    AccumArrayType(type &&) noexcept = default;
    AccumArrayType(type const &) noexcept = default;

    type & operator=(type &&) noexcept = default;
    type & operator=(type const &) noexcept = default;

    std::size_t structSizeInBytes() const noexcept
    { return DynamicPackingInfo<Ts...>::sizeFromAccum(*this); }

    template <std::size_t I>
    std::size_t elemOffset() const noexcept
    { return DynamicPackingInfo<Ts...>::elemOffset<I>(*this); }

    template <std::size_t I>
    void * voidPtr(void * const data) const noexcept
    { return DynamicPackingInfo<Ts...>::voidPtr<I>(data, *this); }

    template <std::size_t I>
    void const * constVoidPtr(void const * const data) const noexcept
    { return DynamicPackingInfo<Ts...>::constVoidPtr<I>(data, *this); }

    template <std::size_t I>
    PointerType<I> ptr(void * const data) const noexcept
    { return DynamicPackingInfo<Ts...>::ptr<I>(data, *this); }

    template <std::size_t I>
    ConstPointerType<I> constPtr(void const * const data) const noexcept
    { return DynamicPackingInfo<Ts...>::constPtr<I>(data, *this); }

    template <std::size_t I>
    ConstReferenceType<I> cref(void const * const data) const noexcept
    { return DynamicPackingInfo<Ts...>::cref<I>(data, *this); }

    template <std::size_t I>
    ReferenceType<I> ref(void * const data) const noexcept
    { return DynamicPackingInfo<Ts...>::ref<I>(data, *this); }

    template <std::size_t I>
    auto get(void const * const data) const noexcept ->
            decltype(DynamicPackingInfo<Ts...>::get<I>(data, *this))
    { return DynamicPackingInfo<Ts...>::get<I>(data, *this); }

    template <std::size_t I>
    auto set(void * const data, ElemType<I> const & v) const noexcept ->
            decltype(DynamicPackingInfo<Ts...>::set<I>(data, *this, v))
    { return DynamicPackingInfo<Ts...>::set<I>(data, *this, v); }

};

} /* namespace sharemind { */

#endif /* SHAREMIND_DYNAMICPACKINGINFO_H */
