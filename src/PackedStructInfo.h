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

#ifndef SHAREMIND_PACKEDSTRUCTINFO_H
#define SHAREMIND_PACKEDSTRUCTINFO_H

#include <cstring>
#include "ConstUnalignedReference.h"
#include "PotentiallyVoidTypeInfo.h"
#include "SizeOfTypes.h"
#include "TemplateCopyTypeParams.h"
#include "TemplateGetTypeParam.h"
#include "TemplatePrefixTypes.h"
#include "UnalignedReference.h"


namespace sharemind {

template <typename ... Ts>
struct PackedStructInfo {

/* Types: */

    using type = PackedStructInfo<Ts...>;

    template <std::size_t I>
    using ElemType = typename TemplateGetTypeParam<I, Ts...>::type;

    template <std::size_t I>
    using ElemOffset =
            typename TemplateCopyTypeParams<
                typename TemplatePrefixTypes<I, Ts...>::type,
                SizeOfTypes
            >::type::type;

    constexpr static std::size_t const size = sizeOfTypes<Ts...>();

    constexpr static std::size_t const numFields = sizeof...(Ts);

/* Methods: */

    template <std::size_t I>
    static ConstUnalignedReference<ElemType<I> > cref(void const * const data)
            noexcept
    { return ptrAdd(data, ElemOffset<I>::value); }

    template <std::size_t I>
    static UnalignedReference<ElemType<I> > ref(void * const data) noexcept
    { return ptrAdd(data, ElemOffset<I>::value); }

    template <std::size_t I>
    static ElemType<I> get(void const * const data) noexcept
    { return cref<I>(data); }

    template <std::size_t I>
    static auto set(void * const data, ElemType<I> const & v) noexcept ->
            decltype(ref<I>(data) = v)
    { return ref<I>(data) = v; }

}; /* struct PackedStructInfo */

#define SHAREMIND_PACKEDSTRUCTINFO_DECLARE_MEMBER_TYPES(...) \
    template <std::size_t I> using ElemType = \
            typename PackedStructInfo<__VA_ARGS__>::template ElemType<I>; \
    template <std::size_t I> using ElemOffset = \
            typename PackedStructInfo<__VA_ARGS__>::template ElemOffset<I>; \
    constexpr static std::size_t const size = \
            PackedStructInfo<__VA_ARGS__>::size; \
    constexpr static std::size_t const numFields = \
            PackedStructInfo<__VA_ARGS__>::numFields; \

#define SHAREMIND_PACKEDSTRUCTINFO_DEFINE_READ_METHODS(...) \
    template <std::size_t I> \
    ConstUnalignedReference<ElemType<I> > cref() const noexcept \
    { return PackedStructInfo<__VA_ARGS__>::template cref<I>(data()); } \
    template <std::size_t I> \
    ConstUnalignedReference<ElemType<I> > ref() const noexcept \
    { return PackedStructInfo<__VA_ARGS__>::template cref<I>(data()); } \
    template <std::size_t I> \
    ElemType<I> get() const noexcept \
    { return PackedStructInfo<__VA_ARGS__>::template get<I>(data()); } \
    bool operator==(type const & rhs) const noexcept \
    { return std::memcmp(data(), rhs.data(), size) == 0; } \
    bool operator!=(type const & rhs) const noexcept \
    { return std::memcmp(data(), rhs.data(), size) != 0; }

#define SHAREMIND_PACKEDSTRUCTINFO_DEFINE_WRITE_METHODS(maybeConst,...) \
    template <std::size_t I> \
    UnalignedReference<ElemType<I> > ref() maybeConst noexcept \
    { return PackedStructInfo<__VA_ARGS__>::template ref<I>(data()); } \
    template <std::size_t I> \
    void set(ElemType<I> const & v) maybeConst noexcept \
    { PackedStructInfo<__VA_ARGS__>::template set<I>(data(), v); } \

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKEDSTRUCTINFO_H */
