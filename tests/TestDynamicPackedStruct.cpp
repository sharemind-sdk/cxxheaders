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

#include "../src/DynamicPackedStruct.h"

#include <cassert>
#include <iostream>

using namespace sharemind;

using R = DynamicFieldPlaceholder;
template <typename T> using A = DynamicVectorFieldPlaceholder<T>;

int main() {
    DynamicPackedStruct<int64_t, char, R, char, A<int32_t>, uint16_t> m(3u, 10u);
    /*std::cout << "Data is at " << m.data() << std::endl;
    std::cout << "Size is " << m.size() << std::endl;
    std::cout << "0u offset is " << m.elemOffset<0u>() << std::endl;
    std::cout << "1u offset is " << m.elemOffset<1u>() << std::endl;
    std::cout << "2u offset is " << m.elemOffset<2u>() << std::endl;
    std::cout << "3u offset is " << m.elemOffset<3u>() << std::endl;
    std::cout << "4u offset is " << m.elemOffset<4u>() << std::endl;
    std::cout << "5u offset is " << m.elemOffset<5u>() << std::endl;
    std::cout << "Accum[0u] is " << m.m_sizes.at(0u) << std::endl;
    std::cout << "Accum[1u] is " << m.m_sizes.at(1u) << std::endl;*/
    assert(m.size() == 55u);
    assert(m.elemOffset<0u>() == 0u);
    assert(m.elemOffset<1u>() == m.elemOffset<0u>() + sizeof(int64_t));
    assert(m.elemOffset<2u>() == m.elemOffset<1u>() + sizeof(char));
    assert(m.elemOffset<3u>() == m.elemOffset<2u>() + 3u);
    assert(m.elemOffset<4u>() == m.elemOffset<3u>() + sizeof(char));
    assert(m.elemOffset<5u>() == m.elemOffset<4u>() + sizeof(int32_t) * 10u);
    char test3[3u] = { '1', '2', '3' };
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
                               UnalignedPointer<int32_t> >::value, "");
    auto const p4 = m.ptr<4u>();
    auto const poly = [](unsigned i) noexcept
                      { return static_cast<int32_t>(3 * i * i + 7 * i + 13); };
    for (unsigned i = 0; i < 10; i++) {
        p4[i] = poly(i);
        assert(p4[i] == poly(i));
        assert(*(p4 + i) == poly(i));
        assert((p4 - 17u)[i + 17u] == poly(i));
    }
    m.set<5u>(42);
    auto const m2 = m;
    assert(m.get<0u>() == m2.get<0u>());
    assert(m.get<1u>() == m2.get<1u>());
    assert(std::memcmp(m2.ptr<2u>(), test3, 3u) == 0);
    assert(m.get<3u>() == m2.get<3u>());
    auto const p42 = m2.constPtr<4u>();
    for (unsigned i = 0; i < 10; i++)
        assert(p42[i] == poly(i));
    assert(m.get<5u>() == m2.get<5u>());
}
