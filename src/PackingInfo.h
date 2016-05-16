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

#ifndef SHAREMIND_PACKINGINFO_H
#define SHAREMIND_PACKINGINFO_H

#include <cstddef>
#include <cstring>
#include "compiler-support/ClangPR26692.h"
#include "ConstUnalignedReference.h"
#include "EnumConstant.h"
#include "PotentiallyVoidTypeInfo.h"
#include "SizeOfTypes.h"
#include "TemplateGetTypeParam.h"
#include "TemplateInstantiateWithTypeParams.h"
#include "TemplatePrefixTypes.h"
#include "TemplateSuffixTypes.h"
#include "UnalignedReference.h"
#include "UnalignedPointer.h"


namespace sharemind {

template <typename ... Ts>
struct PackingInfo {

/* Constants: */

    SHAREMIND_ENUMCONSTANT(std::size_t, size, sizeOfTypes<Ts...>());

    SHAREMIND_ENUMCONSTANT(std::size_t, minSizeInBytes, size);

    SHAREMIND_ENUMCONSTANT(std::size_t, maxSizeInBytes, size);

    SHAREMIND_ENUMCONSTANT(std::size_t, numFields, sizeof...(Ts));

    SHAREMIND_ENUMCONSTANT(std::size_t, numDynamicFields, 0u);

    SHAREMIND_ENUMCONSTANT(bool, hasDynamicFields, false);

/* Types: */

    using type = PackingInfo<Ts...>;

    template <std::size_t I>
    using ElemType = TemplateGetTypeParam_t<I, Ts...>;

    template <std::size_t I>
    using ElemOffset =
            TemplateInstantiateWithTypeParams_t<
                SizeOfTypes,
                TemplatePrefixTypes_t<I, Ts...>
            >;

    template <std::size_t I>
    using ReferenceType = UnalignedReference<ElemType<I> >;

    template <std::size_t I>
    using ConstReferenceType = ConstUnalignedReference<ElemType<I> >;

    template <std::size_t I>
    using PointerType = UnalignedPointer<ElemType<I> >;

    template <std::size_t I>
    using ConstPointerType = UnalignedPointer<ElemType<I> const>;

    template <std::size_t I>
    using PrefixType =
            TemplateInstantiateWithTypeParams_t<
                SHAREMIND_CLANGPR26692_WORKAROUND(sharemind) PackingInfo,
                TemplatePrefixTypes_t<I, Ts...>
            >;

    template <std::size_t I>
    using SuffixType =
            TemplateInstantiateWithTypeParams_t<
                SHAREMIND_CLANGPR26692_WORKAROUND(sharemind) PackingInfo,
                TemplateSuffixTypes_t<I, Ts...>
            >;

    using StaticPrefixType = type;

/* Methods: */

    constexpr static std::size_t sizeInBytes() noexcept { return size; }

    template <std::size_t I>
    static void * voidPtr(void * const data) noexcept
    { return ptrAdd(data, ElemOffset<I>::value); }

    template <std::size_t I>
    static void const * constVoidPtr(void const * const data) noexcept
    { return ptrAdd(data, ElemOffset<I>::value); }

    static void * endVoidPtr(void * const data) noexcept
    { return ptrAdd(data, size); }

    static void const * endVoidPtr(void const * const data) noexcept
    { return ptrAdd(data, size); }

    static void const * endConstVoidPtr(void const * const data) noexcept
    { return ptrAdd(data, size); }

    template <std::size_t I>
    static PointerType<I> ptr(void * const data) noexcept
    { return voidPtr<I>(data); }

    template <std::size_t I>
    static ConstPointerType<I> constPtr(void const * const data) noexcept
    { return constVoidPtr<I>(data); }

    template <std::size_t I>
    static ConstReferenceType<I> cref(void const * const data)
            noexcept
    { return constVoidPtr<I>(data); }

    template <std::size_t I>
    static ReferenceType<I> ref(void * const data) noexcept
    { return voidPtr<I>(data); }

    template <std::size_t I>
    static ElemType<I> get(void const * const data) noexcept
    { return cref<I>(data); }

    template <std::size_t I>
    static auto set(void * const data, ElemType<I> const & v) noexcept ->
            decltype(ref<I>(data) = v)
    { return ref<I>(data) = v; }

}; /* struct PackingInfo */

#define SHAREMIND_PACKINGINFO_DECLARE_MEMBER_CONSTANTS(...) \
    SHAREMIND_ENUMCONSTANT(std::size_t, \
                           staticSize, \
                           PackingInfo<__VA_ARGS__>::size); \
    SHAREMIND_ENUMCONSTANT(std::size_t, \
                           minSizeInBytes, \
                           PackingInfo<__VA_ARGS__>::minSizeInBytes); \
    SHAREMIND_ENUMCONSTANT(std::size_t, \
                           maxSizeInBytes, \
                           PackingInfo<__VA_ARGS__>::maxSizeInBytes); \
    SHAREMIND_ENUMCONSTANT(std::size_t, \
                           numFields, \
                           PackingInfo<__VA_ARGS__>::numFields); \
    SHAREMIND_ENUMCONSTANT(std::size_t, \
                           numDynamicFields, \
                           PackingInfo<__VA_ARGS__>::numDynamicFields); \
    SHAREMIND_ENUMCONSTANT(bool, \
                           hasDynamicFields, \
                           PackingInfo<__VA_ARGS__>::hasDynamicFields);

#define SHAREMIND_PACKINGINFO_DECLARE_MEMBER_TYPES(...) \
    template <std::size_t I> using ElemType = \
            typename PackingInfo<__VA_ARGS__>::template ElemType<I>; \
    template <std::size_t I> using ElemOffset = \
            typename PackingInfo<__VA_ARGS__>::template ElemOffset<I>; \
    template <std::size_t I> using ReferenceType = \
            typename PackingInfo<__VA_ARGS__>::template ReferenceType<I>; \
    template <std::size_t I> using ConstReferenceType = \
            typename PackingInfo<__VA_ARGS__>::template ConstReferenceType<I>; \
    template <std::size_t I> using PointerType = \
            typename PackingInfo<__VA_ARGS__>::template PointerType<I>; \
    template <std::size_t I> using ConstPointerType = \
            typename PackingInfo<__VA_ARGS__>::template ConstPointerType<I>;

#define SHAREMIND_PACKINGINFO_DEFINE_READ_METHODS(...) \
    constexpr static std::size_t size() noexcept \
    { return PackingInfo<__VA_ARGS__>::size; } \
    template <std::size_t I> \
    void const * voidPtr() const noexcept \
    { return PackingInfo<__VA_ARGS__>::template constVoidPtr<I>(data()); } \
    template <std::size_t I> \
    void const * constVoidPtr() const noexcept \
    { return PackingInfo<__VA_ARGS__>::template constVoidPtr<I>(data()); } \
    void const * endVoidPtr() const noexcept \
    { return PackingInfo<__VA_ARGS__>::endConstVoidPtr(data()); } \
    void const * endConstVoidPtr() const noexcept \
    { return PackingInfo<__VA_ARGS__>::endConstVoidPtr(data()); } \
    template <std::size_t I> \
    ConstPointerType<I> ptr() const noexcept \
    { return PackingInfo<__VA_ARGS__>::template constPtr<I>(data()); } \
    template <std::size_t I> \
    ConstPointerType<I> constPtr() const noexcept \
    { return PackingInfo<__VA_ARGS__>::template constPtr<I>(data()); } \
    template <std::size_t I> \
    ConstReferenceType<I> cref() const noexcept \
    { return PackingInfo<__VA_ARGS__>::template cref<I>(data()); } \
    template <std::size_t I> \
    ConstReferenceType<I> ref() const noexcept \
    { return PackingInfo<__VA_ARGS__>::template cref<I>(data()); } \
    template <std::size_t I> \
    ElemType<I> get() const noexcept \
    { return PackingInfo<__VA_ARGS__>::template get<I>(data()); } \
    bool operator==(type const & rhs) const noexcept \
    { return std::memcmp(data(), rhs.data(), staticSize) == 0; } \
    bool operator!=(type const & rhs) const noexcept \
    { return std::memcmp(data(), rhs.data(), staticSize) != 0; }

#define SHAREMIND_PACKINGINFO_DEFINE_WRITE_METHODS(maybeConst,...) \
    template <std::size_t I> \
    void * voidPtr() maybeConst noexcept \
    { return PackingInfo<__VA_ARGS__>::template voidPtr<I>(data()); } \
    void * endVoidPtr() noexcept \
    { return PackingInfo<__VA_ARGS__>::endVoidPtr(data()); } \
    template <std::size_t I> \
    PointerType<I> ptr() maybeConst noexcept \
    { return PackingInfo<__VA_ARGS__>::template ptr<I>(data()); } \
    template <std::size_t I> \
    ReferenceType<I> ref() maybeConst noexcept \
    { return PackingInfo<__VA_ARGS__>::template ref<I>(data()); } \
    template <std::size_t I> \
    void set(ElemType<I> const & v) maybeConst noexcept \
    { PackingInfo<__VA_ARGS__>::template set<I>(data(), v); } \

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKINGINFO_H */
