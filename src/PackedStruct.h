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

#ifndef SHAREMIND_PACKEDSTRUCT_H
#define SHAREMIND_PACKEDSTRUCT_H

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>
#include "ConstUnalignedReference.h"
#include "PotentiallyVoidTypeInfo.h"
#include "SizeOfTypes.h"
#include "TemplateCopyParams.h"
#include "TemplateGetTypeParam.h"
#include "TemplatePrefixTypes.h"
#include "UnalignedReference.h"


namespace sharemind {

template <typename ... Ts>
class __attribute__((packed)) PackedStruct {

public: /* Types: */

    using type = PackedStruct<Ts...>;

    template <size_t I>
    using ElemType = typename TemplateGetTypeParam<I, Ts...>::type;

    template <size_t I>
    using ElemOffset =
            typename TemplateCopyParams<
                typename TemplatePrefixTypes<I, Ts...>::type,
                SizeOfTypes
            >::type::type;

    constexpr static size_t const size = sizeOfTypes<Ts...>();

    constexpr static size_t const numFields = sizeof...(Ts);

public: /* Methods: */

    template <size_t I>
    ConstUnalignedReference<ElemType<I> > cref() const noexcept
    { return ptrAdd(m_data.asUint8, ElemOffset<I>::value); }

    template <size_t I>
    UnalignedReference<ElemType<I> > ref() noexcept
    { return ptrAdd(m_data.asUint8, ElemOffset<I>::value); }

    template <size_t I>
    ConstUnalignedReference<ElemType<I> > ref() const noexcept
    { return cref(); }

    template <size_t I>
    ElemType<I> get() const noexcept { return *cref<I>(); }

    template <size_t I>
    void set(ElemType<I> v) noexcept { return ref<I>() = std::move(v); }

    template <size_t I, typename T>
    void set(T && v) noexcept { return ref<I>() = std::forward<T>(v); }

    void * data() noexcept { return &m_data; }
    void const * data() const noexcept { return &m_data; }

    uint8_t * dataUint8() noexcept { return m_data.asUint8; }
    uint8_t const * dataUint8() const noexcept { return m_data.asUint8; }

    char * dataChar() noexcept { return m_data.asChar; }
    char const * dataChar() const noexcept { return m_data.asChar; }

    unsigned char * dataUnsignedChar() noexcept { return m_data.asUChar; }
    unsigned char const * dataUnsignedChar() const noexcept { return m_data.asUChar; }

    bool operator==(type const & rhs) const noexcept
    { return std::memcmp(&m_data, &rhs.m_data, size) == 0; }

    bool operator!=(type const & rhs) const noexcept
    { return std::memcmp(&m_data, &rhs.m_data, size) != 0; }

private: /* Fields: */

    union __attribute__((packed)) {
        uint8_t asUint8[size];
        char asChar[size];
        unsigned char asUChar[size];
    } __attribute__((packed)) m_data;
    static_assert(sizeof(m_data) == size, "");

};

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKEDSTRUCT_H */

#ifdef SHAREMIND_PACKEDSTRUCT_TEST

#include <array>
#include <cassert>


using sharemind::PackedStruct;

template <typename Msg, std::size_t N = Msg::numFields>
struct PackedStructEqualityChecker;

template <typename Msg>
struct PackedStructEqualityChecker<Msg, 0u>
{ static void check(Msg const &, Msg const &) noexcept {} };

template <typename Msg, std::size_t I>
struct PackedStructEqualityChecker {
    static void check(Msg const & a, Msg const & b) noexcept {
        PackedStructEqualityChecker<Msg, I - 1u>::check(a, b);
        assert(a.template get<I - 1u>() == b.template get<I - 1u>());
    }
};

template <typename Msg>
void checkPackedStruct(Msg const & m) noexcept {
    static_assert(std::is_pod<Msg>::value, "");
    static_assert(sizeof(Msg) == Msg::size, "");
    Msg m2;
    std::memcpy(&m2, &m, sizeof(Msg));
    PackedStructEqualityChecker<Msg>::check(m, m2);
}

int main() {
    PackedStruct<char, int, std::array<uint16_t, 10>, float, long> m;
    m.set<0u>('x');
    m.set<1u>(42);
    m.set<2u>({1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u});
    m.set<3u>(55);
    m.set<4u>(231234);
    checkPackedStruct(m);
    PackedStruct<char, decltype(m), char> m2;
    static_assert(sizeof(m2) == sizeof(m) + 2, "");
    m2.set<0u>('<');
    m2.set<1u>(std::move(m));
    m2.set<2u>('>');
    checkPackedStruct(m2);
    return 0;
}

#endif /* SHAREMIND_PACKEDSTRUCT_TEST */
