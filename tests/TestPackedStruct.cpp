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

#include "../src/PackedStruct.h"

#include <array>
#include <cassert>
#include <cstring>
#include <type_traits>
#include <utility>


using sharemind::PackedStruct;

template <typename Msg, std::size_t N = Msg::numFields>
struct PackedStructEqualityChecker;

template <typename Msg>
struct PackedStructEqualityChecker<Msg, 0u>
{ static void check(Msg const &, Msg const &) noexcept {} };

template <typename Msg, std::size_t I>
struct PackedStructEqualityChecker {
    static_assert(
            std::is_same<
                decltype(std::declval<Msg const>().template ref<I - 1u>()),
                decltype(std::declval<Msg const>().template cref<I - 1u>())
            >::value, "");
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

template <typename Lhs, typename Rhs>
void checkPackedRefs(Lhs && lhs, Rhs && rhs) noexcept {
    PackedStructEqualityChecker<typename std::remove_reference<Lhs>::type>
            ::check(lhs, rhs);
}

int main() {
    using Msg1 = PackedStruct<char, int, std::array<uint16_t, 10>, float, long>;
    using Msg2 = PackedStruct<char, Msg1, char>;
    static_assert(sizeof(Msg2) == sizeof(Msg1) + 2, "");
    Msg1 m;
    Msg2 m2;
    using T1 = decltype(std::declval<Msg1 const>().ref<0u>());
    using T2 = decltype(std::declval<Msg1 const>().cref<0u>());
    static_assert(std::is_same<T1, T2>::value, "");

    // Regular checks
    m.set<0u>('x');
    m.set<1u>(42);
    m.set<2u>({{1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u}});
    m.set<3u>(55);
    m.set<4u>(231234);
    checkPackedStruct(m);
    m2.set<0u>('<');
    m2.set<1u>(std::move(m));
    m2.set<2u>('>');
    checkPackedStruct(m2);

    // Checks using references:
    auto const mRefs = m.refs();
    mRefs.set<0u>('x');
    mRefs.set<1u>(42);
    mRefs.set<2u>({{1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u}});
    mRefs.set<3u>(55);
    mRefs.set<4u>(231234);
    checkPackedRefs(m.crefs(), std::move(mRefs));
    auto const m2Refs = m2.refs();
    m2Refs.set<0u>('<');
    m2Refs.set<1u>(std::move(m));
    m2Refs.set<2u>('>');
    checkPackedRefs(m2.crefs(), std::move(m2Refs));
}
