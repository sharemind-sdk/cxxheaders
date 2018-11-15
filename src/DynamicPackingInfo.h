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

#ifndef SHAREMIND_DYNAMICPACKINGINFO_H
#define SHAREMIND_DYNAMICPACKINGINFO_H

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
#include "ConstUnalignedReference.h"
#include "PackingInfo.h"
#include "SizeOfTypes.h"
#include "TemplateCommonPrefixTypes.h"
#include "TemplateFilterTypes.h"
#include "TemplateGetTypeParam.h"
#include "TemplateInstantiateWithTypeParams.h"
#include "TemplatePrefixTypes.h"
#include "TemplateSuffixTypes.h"
#include "UnalignedPointer.h"
#include "UnalignedReference.h"


namespace sharemind {

template <std::size_t min_ = std::numeric_limits<std::size_t>::min(),
          std::size_t max_ = std::numeric_limits<std::size_t>::max()>
struct DynamicFieldPlaceholder {
    using type = DynamicFieldPlaceholder;
    constexpr static std::size_t min() { return min_; }
    constexpr static std::size_t max() { return max_; }
    constexpr static std::size_t minBytes() { return min_; }
    constexpr static std::size_t maxBytes() { return max_; }
};

template <typename T,
          std::size_t min_ = std::numeric_limits<std::size_t>::min(),
          std::size_t max_ = std::numeric_limits<std::size_t>::max()>
struct DynamicVectorFieldPlaceholder {
    using type = DynamicVectorFieldPlaceholder<T>;
    using valueType = T;
    constexpr static std::size_t min() { return min_; }
    constexpr static std::size_t max() { return max_; }
    constexpr static std::size_t minBytes() { return min_ * sizeof(valueType); }
    constexpr static std::size_t maxBytes() { return max_ * sizeof(valueType); }
};

namespace Detail {
namespace DynamicPacking {

template <typename T>
struct FieldTraits {
    using ReferenceType = UnalignedReference<T>;
    using ConstReferenceType = ConstUnalignedReference<T>;
    using PointerType = UnalignedPointer<T>;
    using ConstPointerType = UnalignedPointer<T const>;
    constexpr static bool isStatic = true;
    constexpr static std::size_t min = 1u;
    constexpr static std::size_t max = 1u;
    constexpr static std::size_t minBytes = sizeof(T);
    constexpr static std::size_t maxBytes = sizeof(T);
};

template <std::size_t min_, std::size_t max_>
struct FieldTraits<DynamicFieldPlaceholder<min_, max_> > {
    using PointerType = void *;
    using ConstPointerType = void const *;
    constexpr static bool isStatic = false;
    constexpr static std::size_t min =
            DynamicFieldPlaceholder<min_, max_>::min();
    constexpr static std::size_t max =
            DynamicFieldPlaceholder<min_, max_>::max();
    constexpr static std::size_t minBytes =
            DynamicFieldPlaceholder<min_, max_>::minBytes();
    constexpr static std::size_t maxBytes =
            DynamicFieldPlaceholder<min_, max_>::maxBytes();
};

template <typename T, std::size_t min_, std::size_t max_>
struct FieldTraits<DynamicVectorFieldPlaceholder<T, min_, max_> > {
    using ReferenceType = UnalignedReference<T>;
    using ConstReferenceType = ConstUnalignedReference<T>;
    using PointerType = UnalignedPointer<T>;
    using ConstPointerType = UnalignedPointer<T const>;
    constexpr static bool isStatic = false;
    constexpr static std::size_t min =
            DynamicVectorFieldPlaceholder<T, min_, max_>::min();
    constexpr static std::size_t max =
            DynamicVectorFieldPlaceholder<T, min_, max_>::max();
    constexpr static std::size_t minBytes =
            DynamicVectorFieldPlaceholder<T, min_, max_>::minBytes();
    constexpr static std::size_t maxBytes =
            DynamicVectorFieldPlaceholder<T, min_, max_>::maxBytes();
};

template <typename ... Ts> struct FieldsTraits;

template <>
struct FieldsTraits<> {
    constexpr static bool isStatic = true;
    constexpr static std::size_t minBytes = 0u;
    constexpr static std::size_t maxBytes = 0u;
};

template <typename T, typename ... Ts>
struct FieldsTraits<T, Ts...> {
    constexpr static bool isStatic =
            FieldTraits<T>::isStatic && FieldsTraits<Ts...>::isStatic;
    constexpr static std::size_t minBytes =
            FieldTraits<T>::minBytes + FieldsTraits<Ts...>::minBytes;
    constexpr static std::size_t maxBytes =
            FieldTraits<T>::maxBytes + FieldsTraits<Ts...>::maxBytes;
};

template <typename ... Ts> struct ValidSizes;

template <> struct ValidSizes<>
{ constexpr static bool isValid() noexcept { return true; } };

template <typename T, typename ... Ts>
struct ValidSizes<T, Ts...> {

    template <typename ... Args>
    constexpr static bool isValid(std::size_t const size, Args && ... args)
            noexcept
    {
        /* Ignore -Wtype-limits "comparison is always true" warnings if min/max
           are SIZE_MIN/SIZE_MAX, which is the default. */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wtype-limits"
        return (size >= FieldTraits<T>::min)
               && (size <= FieldTraits<T>::max)
               && ValidSizes<Ts...>::isValid(std::forward<Args>(args)...);
        #pragma GCC diagnostic pop
    }

};

template <typename T>
using StaticFieldPred =
        std::integral_constant<bool, FieldTraits<T>::isStatic>;

template <typename ... Ts>
using StaticFieldFilter = TemplateFilterTypes_t<StaticFieldPred, Ts...>;

template <typename T>
using DynamicFieldPred =
        std::integral_constant<bool, !FieldTraits<T>::isStatic>;

template <typename ... Ts>
using DynamicFieldFilter = TemplateFilterTypes_t<DynamicFieldPred, Ts...>;

template <typename ... Ts> struct AccumSize;

template <>
struct AccumSize<>
{ constexpr static std::size_t value() noexcept { return 0u; } };

template <std::size_t min_, std::size_t max_, typename ... Ts>
struct AccumSize<DynamicFieldPlaceholder<min_, max_>, Ts...> {
    template <typename ... Args>
    constexpr static std::size_t value(std::size_t const size,
                                       Args && ... args) noexcept
    { return size + AccumSize<Ts...>::value(std::forward<Args>(args)...); }
};

template <typename T, std::size_t min_, std::size_t max_, typename ... Ts>
struct AccumSize<DynamicVectorFieldPlaceholder<T, min_, max_>, Ts...> {
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
            DynamicFieldFilter<Ts...>::size() <= 0u,
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

template <std::size_t min_, std::size_t max_, typename ... Ts>
struct AccumVecPopulator<DynamicFieldPlaceholder<min_, max_>, Ts...> {
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

template <typename T, std::size_t min_, std::size_t max_, typename ... Ts>
struct AccumVecPopulator<DynamicVectorFieldPlaceholder<T, min_, max_>, Ts...> {
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


template <typename ... Ts>
struct AccumArrayType
        : std::array<std::size_t, DynamicFieldFilter<Ts...>::size()>
{

/* Constants: */

    constexpr static std::size_t minSizeInBytes()
    { return FieldsTraits<Ts...>::minBytes; }

    constexpr static std::size_t maxSizeInBytes()
    { return FieldsTraits<Ts...>::maxBytes; }

    constexpr static std::size_t numFields() { return sizeof...(Ts); }

    constexpr static std::size_t numDynamicFields()
    { return DynamicFieldFilter<Ts...>::size(); }

    constexpr static bool hasDynamicFields() { return numDynamicFields() > 0u; }

/* Types: */

    using type = AccumArrayType;

    using base = std::array<std::size_t, numDynamicFields()>;

    template <std::size_t I>
    using ElemType = TemplateGetTypeParam_t<I, Ts...>;

    template <std::size_t I>
    using TypeTraits = FieldTraits<ElemType<I> >;

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
    AccumArrayType(Args && ... args) noexcept {
        static_assert(sizeof...(Args) == numDynamicFields(),
                      "The number of arguments must match the number of "
                      "dynamic fields.");
        using P = AccumVecPopulator<Ts...>;
        P::populate(this->data(), 0u, std::forward<Args>(args)...);
    }

    AccumArrayType(type & copy) noexcept
        : AccumArrayType(const_cast<type const &>(copy))
    {}

    AccumArrayType(type &&) noexcept = default;
    AccumArrayType(type const &) noexcept = default;

    type & operator=(type &&) noexcept = default;
    type & operator=(type const &) noexcept = default;

    std::size_t structSizeInBytes() const noexcept
    { return StaticTailSize<Ts...>::value + (*this)[numDynamicFields() - 1u]; }

    template <std::size_t I>
    typename std::enable_if<
        (TemplateInstantiateWithTypeParams_t<
                DynamicFieldFilter,
                TemplatePrefixTypes_t<I, Ts...>
            >::size() > 0u),
        std::size_t
    >::type
    elemOffset() const noexcept
    {
        return (*this)[
                TemplateInstantiateWithTypeParams_t<
                    DynamicFieldFilter,
                    TemplatePrefixTypes_t<I, Ts...>
                >::size() - 1u]
               + TemplateInstantiateWithTypeParams_t<
                    StaticTailSize,
                    TemplatePrefixTypes_t<I, Ts...>
                >::value;
    }

    template <std::size_t I>
    typename std::enable_if<
        !(TemplateInstantiateWithTypeParams_t<
                  DynamicFieldFilter,
                  TemplatePrefixTypes_t<I, Ts...>
              >::size() > 0u),
        std::size_t
    >::type
    elemOffset() const noexcept {
        return TemplateInstantiateWithTypeParams_t<
                    SizeOfTypes,
                    TemplatePrefixTypes_t<I, Ts...>
                >::value;
    }

    template <std::size_t I>
    void * voidPtr(void * const data) const noexcept
    { return ptrAdd(data, elemOffset<I>()); }

    template <std::size_t I>
    void const * constVoidPtr(void const * const data) const noexcept
    { return ptrAdd(data, elemOffset<I>()); }

    void * endVoidPtr(void * const data) const noexcept
    { return ptrAdd(data, structSizeInBytes()); }

    void const * endVoidPtr(void const * const data) const noexcept
    { return ptrAdd(data, structSizeInBytes()); }

    void const * endConstVoidPtr(void const * const data) const noexcept
    { return ptrAdd(data, structSizeInBytes()); }

    template <std::size_t I>
    PointerType<I> ptr(void * const data) const noexcept
    { return voidPtr<I>(data); }

    template <std::size_t I>
    ConstPointerType<I> constPtr(void const * const data) const noexcept
    { return constVoidPtr<I>(data); }

    template <std::size_t I>
    ConstReferenceType<I> cref(void const * const data) const noexcept
    { return constVoidPtr<I>(data); }

    template <std::size_t I>
    ReferenceType<I> ref(void * const data) const noexcept
    { return voidPtr<I>(data); }

    template <std::size_t I>
    typename std::enable_if<TypeTraits<I>::isStatic, ElemType<I> >::type
    get(void const * const data) const noexcept
    { return cref<I>(data); }

    template <std::size_t I>
    auto set(void * const data, ElemType<I> const & v) const noexcept ->
            typename std::enable_if<
                TypeTraits<I>::isStatic,
                decltype(ref<I>(data) = v)
            >::type
    { return ref<I>(data) = v; }

}; /* struct AccumArrayType */

} /* namespace DynamicPacking { */
} /* namespace Detail { */

template <typename ... Ts> struct DynamicPackingInfo;

template <typename ... Ts>
struct DynamicPackingInfo {

/* Constants: */

    constexpr static std::size_t minSizeInBytes()
    { return Detail::DynamicPacking::FieldsTraits<Ts...>::minBytes; }

    constexpr static std::size_t maxSizeInBytes()
    { return Detail::DynamicPacking::FieldsTraits<Ts...>::maxBytes; }

    constexpr static std::size_t numFields() { return sizeof...(Ts); }

    constexpr static std::size_t numDynamicFields()
    { return Detail::DynamicPacking::DynamicFieldFilter<Ts...>::size(); }

    constexpr static bool hasDynamicFields() { return numDynamicFields() > 0u; }

/* Types: */

    using type = DynamicPackingInfo<Ts...>;

    using AccumArrayType = Detail::DynamicPacking::AccumArrayType<Ts...>;

    template <std::size_t I>
    using ElemType = typename AccumArrayType::template ElemType<I>;

    template <std::size_t I>
    using TypeTraits = typename AccumArrayType::template TypeTraits<I>;

    template <std::size_t I>
    using ReferenceType = typename TypeTraits<I>::ReferenceType;

    template <std::size_t I>
    using ConstReferenceType = typename TypeTraits<I>::ConstReferenceType;

    template <std::size_t I>
    using PointerType = typename TypeTraits<I>::PointerType;

    template <std::size_t I>
    using ConstPointerType = typename TypeTraits<I>::ConstPointerType;

    template <std::size_t I>
    using PrefixType =
            TemplateInstantiateWithTypeParams_t<
                DynamicPackingInfo,
                TemplatePrefixTypes_t<I, Ts...>
            >;

    template <std::size_t I>
    using SuffixType =
            TemplateInstantiateWithTypeParams_t<
                DynamicPackingInfo,
                TemplateSuffixTypes_t<I, Ts...>
            >;

    using StaticPrefixType =
            TemplateInstantiateWithTypeParams_t<
                PackingInfo,
                TemplateCommonPrefixTypes_t<
                    TemplateTypeList<Ts...>,
                    Detail::DynamicPacking::StaticFieldFilter<Ts...>
                >
            >;

/* Methods: */

    template <typename ... Args>
    constexpr static bool validSizes(Args && ... args) noexcept {
        return TemplateInstantiateWithTypeParams_t<
                    Detail::DynamicPacking::ValidSizes,
                    Detail::DynamicPacking::DynamicFieldFilter<Ts...>
                >::isValid(std::forward<Args>(args)...);
    }

    template <typename ... Args>
    constexpr static std::size_t sizeInBytes(Args && ... args) noexcept {
        return Detail::DynamicPacking::AccumSize<Ts...>::value(
                    std::forward<Args>(args)...);
    }

    template <typename ... Args>
    static void populateAccumArray(std::size_t * const accumSizes,
                                   Args && ... args) noexcept
    {
        static_assert(sizeof...(Args) == numDynamicFields(),
                      "The number of arguments must match the number of "
                      "dynamic fields.");
        using P = Detail::DynamicPacking::AccumVecPopulator<Ts...>;
        P::populate(accumSizes, 0u, std::forward<Args>(args)...);
    }

    static constexpr std::size_t sizeFromAccum(AccumArrayType const & accumSizes)
            noexcept
    {
        return Detail::DynamicPacking::StaticTailSize<Ts...>::value
               + accumSizes[numDynamicFields() - 1u];
    }

    template <std::size_t I>
    constexpr static std::size_t elemOffset(AccumArrayType const & accumSizes)
            noexcept
    { return accumSizes.template elemOffset<I>(); }

    template <std::size_t I>
    constexpr static void * voidPtr(void * const data,
                                    AccumArrayType const & accumSizes) noexcept
    { return accumSizes.template voidPtr<I>(data); }

    template <std::size_t I>
    constexpr static void const * constVoidPtr(
            void const * const data,
            AccumArrayType const & accumSizes) noexcept
    { return accumSizes.template constVoidPtr<I>(data); }

    constexpr static void * endVoidPtr(void * const data,
                                       AccumArrayType const & accumSizes)
            noexcept
    { return accumSizes.endVoidPtr(data); }

    constexpr static void const * endVoidPtr(void const * const data,
                                             AccumArrayType const & accumSizes)
            noexcept
    { return accumSizes.endConstVoidPtr(data); }

    constexpr static void const * endConstVoidPtr(
            void const * const data,
            AccumArrayType const & accumSizes) noexcept
    { return accumSizes.endConstVoidPtr(data); }

    template <std::size_t I>
    constexpr static PointerType<I> ptr(void * const data,
                                        AccumArrayType const & accumSizes)
            noexcept
    { return voidPtr<I>(data, accumSizes); }

    template <std::size_t I, typename AccumSizes>
    constexpr static ConstPointerType<I> constPtr(void const * const data,
                                                  AccumSizes const & accumSizes)
            noexcept
    { return constVoidPtr<I>(data, accumSizes); }

    template <std::size_t I>
    constexpr static ConstReferenceType<I> cref(
            void const * const data,
            AccumArrayType const & accumSizes) noexcept
    { return constVoidPtr<I>(data, accumSizes); }

    template <std::size_t I>
    constexpr static ReferenceType<I> ref(void * const data,
                                          AccumArrayType const & accumSizes)
            noexcept
    { return voidPtr<I>(data, accumSizes); }

    template <std::size_t I>
    constexpr static auto get(void const * const data,
                              AccumArrayType const & accumSizes) noexcept ->
            decltype(accumSizes.template get<I>(data))
    { return accumSizes.template get<I>(data); }

    template <std::size_t I>
    constexpr static auto set(void * const data,
                              AccumArrayType const & accumSizes,
                              ElemType<I> const & v) noexcept ->
            decltype(accumSizes.template set<I>(data, v))
    { return accumSizes.template set<I>(data, v); }

}; /* struct DynamicPackingInfo */

} /* namespace sharemind { */

#endif /* SHAREMIND_DYNAMICPACKINGINFO_H */
