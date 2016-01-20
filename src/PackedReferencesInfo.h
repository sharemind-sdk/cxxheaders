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

#ifndef SHAREMIND_PACKEDREFERENCESINFO_H
#define SHAREMIND_PACKEDREFERENCESINFO_H

#include <cstring>
#include "ConstUnalignedReference.h"
#include "PotentiallyVoidTypeInfo.h"
#include "SizeOfTypes.h"
#include "TemplateCopyParams.h"
#include "TemplateGetTypeParam.h"
#include "TemplatePrefixTypes.h"
#include "UnalignedReference.h"


namespace sharemind {

template <typename ... Ts>
struct PackedReferencesInfo {

/* Types: */

    using type = PackedReferencesInfo<Ts...>;

    template <std::size_t I>
    using ElemType = typename TemplateGetTypeParam<I, Ts...>::type;

    template <std::size_t I>
    using ElemOffset =
            typename TemplateCopyParams<
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
    { return *cref<I>(data); }

    template <std::size_t I>
    static void set(void * const data, ElemType<I> const & v) noexcept
    { return ref<I>(data) = v; }

}; /* struct PackedReferencesInfo */

#define SHAREMIND_PACKEDREFERENCESINFO_DECLARE_MEMBER_TYPES(...) \
    private: /* Types: */ \
        using PackedReferencesInfo_ = PackedReferencesInfo<__VA_ARGS__>; \
    public: /* Types: */ \
        template <std::size_t I> \
        using ElemType = typename PackedReferencesInfo_::template ElemType<I>; \
        template <std::size_t I> \
        using ElemOffset = \
                typename PackedReferencesInfo_::template ElemOffset<I>; \
        constexpr static std::size_t const size = PackedReferencesInfo_::size; \
        constexpr static std::size_t const numFields = \
                PackedReferencesInfo_::numFields; \
    private:

#define SHAREMIND_PACKEDREFERENCESINFO_DEFINE_READ_METHODS \
    template <std::size_t I> \
    ConstUnalignedReference<ElemType<I> > cref() const noexcept \
    { return PackedReferencesInfo_::template cref<I>(data()); } \
    template <std::size_t I> \
    ConstUnalignedReference<ElemType<I> > ref() const noexcept \
    { return PackedReferencesInfo_::template cref<I>(data()); } \
    template <std::size_t I> \
    ElemType<I> get() const noexcept \
    { return PackedReferencesInfo_::template get<I>(data()); } \
    bool operator==(type const & rhs) const noexcept \
    { return std::memcmp(data(), rhs.data(), size) == 0; } \
    bool operator!=(type const & rhs) const noexcept \
    { return std::memcmp(data(), rhs.data(), size) != 0; }

#define SHAREMIND_PACKEDREFERENCESINFO_DEFINE_WRITE_METHODS(maybeConst) \
    template <std::size_t I> \
    UnalignedReference<ElemType<I> > ref() maybeConst noexcept \
    { return PackedReferencesInfo_::template ref<I>(data()); } \
    template <std::size_t I> \
    void set(ElemType<I> const & v) maybeConst noexcept \
    { PackedReferencesInfo_::template set<I>(data(), v); } \

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKEDREFERENCESINFO_H */
