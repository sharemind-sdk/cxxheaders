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

#include "../src/DynamicPackedStruct.h"

#include <cstdint>
#include <cstring>
#include <type_traits>
#include "../src/DynamicPackingInfo.h"
#include "../src/PotentiallyVoidTypeInfo.h"
#include "../src/UnalignedPointer.h"
#include "../src/UnalignedReference.h"
#include "../src/TestAssert.h"


using namespace sharemind;

static_assert(DynamicPackedStruct<>::minSizeInBytes() == 0u, "");
static_assert(DynamicPackedStruct<>::maxSizeInBytes() == 0u, "");
static_assert(DynamicPackedStruct<>::numFields() == 0u, "");
static_assert(DynamicPackedStruct<>::numDynamicFields() == 0u, "");
static_assert(DynamicPackedStruct<>::hasDynamicFields() == false, "");

using R = DynamicFieldPlaceholder<>;
template <typename T> using A = DynamicVectorFieldPlaceholder<T>;

template <typename ...> struct T {};
static_assert(
        std::is_same<
            DynamicPackedStructFromTemplate_t<T<int, char> >,
            DynamicPackedStruct<int, char>
        >::value, "");

static_assert(
        std::is_same<
            DynamicPackedStruct<std::int64_t, char, R, char, A<std::int32_t>, std::uint16_t>::PrefixType<3u>,
            DynamicPackedStruct<std::int64_t, char, R>
        >::value, "");
static_assert(
        std::is_same<
            typename DynamicPackedStruct<std::int64_t, char, R, char, A<std::int32_t>, std::uint16_t>::StaticPrefixType,
            DynamicPackedStruct<std::int64_t, char>
        >::value, "");

int main() {
    DynamicPackedStruct<std::int64_t, char, R, char, A<std::int32_t>, std::uint16_t> m(3u, 10u);
    decltype(m) const & cm = m;
#if 0
    std::cout << "Data is at " << m.data() << std::endl;
    std::cout << "Size is " << m.size() << std::endl;
    std::cout << "0u offset is " << m.elemOffset<0u>() << std::endl;
    std::cout << "1u offset is " << m.elemOffset<1u>() << std::endl;
    std::cout << "2u offset is " << m.elemOffset<2u>() << std::endl;
    std::cout << "3u offset is " << m.elemOffset<3u>() << std::endl;
    std::cout << "4u offset is " << m.elemOffset<4u>() << std::endl;
    std::cout << "5u offset is " << m.elemOffset<5u>() << std::endl;
    std::cout << "Accum[0u] is " << m.m_sizes.at(0u) << std::endl;
    std::cout << "Accum[1u] is " << m.m_sizes.at(1u) << std::endl;
#endif
    SHAREMIND_TESTASSERT(m.size() == 55u);
    static_assert(std::is_same<decltype(m.endVoidPtr()), void *>::value, "");
    static_assert(
        std::is_same<decltype(cm.endVoidPtr()), void const *>::value, "");
    static_assert(
        std::is_same<decltype(m.endConstVoidPtr()), void const *>::value, "");
    static_assert(
        std::is_same<decltype(cm.endConstVoidPtr()), void const *>::value, "");
    SHAREMIND_TESTASSERT(m.endVoidPtr() == cm.endVoidPtr());
    SHAREMIND_TESTASSERT(m.endVoidPtr() == m.endConstVoidPtr());
    SHAREMIND_TESTASSERT(m.endVoidPtr() == cm.endConstVoidPtr());
    SHAREMIND_TESTASSERT(ptrAdd(m.data(), m.size()) == m.endVoidPtr());

    SHAREMIND_TESTASSERT(m.elemOffset<0u>() == 0u);
    SHAREMIND_TESTASSERT(m.elemOffset<1u>() == m.elemOffset<0u>() + sizeof(std::int64_t));
    SHAREMIND_TESTASSERT(m.elemOffset<2u>() == m.elemOffset<1u>() + sizeof(char));
    SHAREMIND_TESTASSERT(m.elemOffset<3u>() == m.elemOffset<2u>() + 3u);
    SHAREMIND_TESTASSERT(m.elemOffset<4u>() == m.elemOffset<3u>() + sizeof(char));
    SHAREMIND_TESTASSERT(m.elemOffset<5u>() == m.elemOffset<4u>() + sizeof(std::int32_t) * 10u);
    char const test3[3u] = { '1', '2', '3' };
    m.set<0u>(42);
    m.set<1u>('X');
    static_assert(std::is_same<decltype(m.ptr<2u>()), void *>::value, "");
    std::memcpy(m.ptr<2u>(), test3, 3u);
    (void) m.ref<3u>();
    (void) m.cref<3u>();
    m.set<3u>('Y');
    (void) m.ref<4u>();
    (void) m.cref<4u>();
    static_assert(std::is_same<decltype(m.ptr<4u>()),
                               UnalignedPointer<std::int32_t> >::value, "");
    auto const p4 = m.ptr<4u>();
    auto const poly = [](unsigned i) noexcept
                      { return static_cast<std::int32_t>(3 * i * i + 7 * i + 13); };
    for (unsigned i = 0; i < 10; i++) {
        p4[i] = poly(i);
        SHAREMIND_TESTASSERT(p4[i] == poly(i));
        SHAREMIND_TESTASSERT(*(p4 + i) == poly(i));
        SHAREMIND_TESTASSERT((p4 - 17u)[i + 17u] == poly(i));
    }
    m.set<5u>(42);
    auto const m2 = m;
    SHAREMIND_TESTASSERT(m.get<0u>() == m2.get<0u>());
    SHAREMIND_TESTASSERT(m.get<1u>() == m2.get<1u>());
    SHAREMIND_TESTASSERT(std::memcmp(m2.ptr<2u>(), test3, 3u) == 0);
    SHAREMIND_TESTASSERT(m.get<3u>() == m2.get<3u>());
    #ifndef NDEBUG
    auto const p42 = m2.constPtr<4u>();
    for (unsigned i = 0; i < 10; i++)
        SHAREMIND_TESTASSERT(p42[i] == poly(i));
    #endif
    SHAREMIND_TESTASSERT(m.get<5u>() == m2.get<5u>());
}
