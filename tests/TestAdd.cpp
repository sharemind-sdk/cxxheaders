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

#include "../src/Add.h"

#include <cstddef>
#include <type_traits>
#include <utility>


// Value node:
template <std::size_t v>
struct V: std::integral_constant<std::size_t, v> {};

// Sum node:
template <typename LHS, typename RHS>
struct S {
    using lhs = LHS;
    using rhs = RHS;
};

template <std::size_t lv, std::size_t rv>
constexpr S<V<lv>, V<rv> > operator+(V<lv> const &, V<rv> const &)
{ return S<V<lv>, V<rv> >(); }

template <std::size_t lv, typename LHS, typename RHS>
constexpr S<V<lv>, S<LHS, RHS> > operator+(V<lv> const &, S<LHS, RHS> const &)
{ return S<V<lv>, S<LHS, RHS> >(); }

template <typename LHS, typename RHS, std::size_t rv>
constexpr S<S<LHS, RHS>, V<rv> > operator+(S<LHS, RHS> const &, V<rv> const &)
{ return S<S<LHS, RHS>, V<rv> >(); }

template <typename LHS, typename RHS, typename LHS2, typename RHS2>
constexpr S<S<LHS, RHS>, S<LHS2, RHS2> > operator+(S<LHS, RHS> const &,
                                                   S<LHS2, RHS2> const &)
{ return S<S<LHS, RHS>, S<LHS2, RHS2> >(); }


template <typename Expected, typename ... Args>
constexpr bool testAdd() noexcept {
    return std::is_same<
                typename std::decay<
                    decltype(sharemind::add(std::declval<Args const &>()...))
                >::type,
                Expected
            >::value;
}

int main() {
    using V1 = V<1u>;
    using V2 = V<2u>;
    using V3 = V<3u>;
    using V4 = V<4u>;
    static_assert(testAdd<V1, V1>(), "");
    static_assert(testAdd<S<V1, V2>, V1, V2>(), "");
    static_assert(testAdd<S<V2, V1>, V2, V1>(), "");
    static_assert(testAdd<S<S<V1, V2>, V3>, V1, V2, V3>(), "");
    static_assert(testAdd<S<S<V1, V3>, V2>, V1, V3, V2>(), "");
    static_assert(testAdd<S<S<V2, V1>, V3>, V2, V1, V3>(), "");
    static_assert(testAdd<S<S<V2, V3>, V1>, V2, V3, V1>(), "");
    static_assert(testAdd<S<S<V3, V1>, V2>, V3, V1, V2>(), "");
    static_assert(testAdd<S<S<V3, V2>, V1>, V3, V2, V1>(), "");
    static_assert(testAdd<S<S<S<V1, V2>, V3>, V4>, V1, V2, V3, V4>(), "");
    static_assert(testAdd<S<S<S<V1, V2>, V4>, V3>, V1, V2, V4, V3>(), "");
    static_assert(testAdd<S<S<S<V1, V3>, V2>, V4>, V1, V3, V2, V4>(), "");
    static_assert(testAdd<S<S<S<V1, V3>, V4>, V2>, V1, V3, V4, V2>(), "");
    static_assert(testAdd<S<S<S<V1, V4>, V2>, V3>, V1, V4, V2, V3>(), "");
    static_assert(testAdd<S<S<S<V1, V4>, V3>, V2>, V1, V4, V3, V2>(), "");
    static_assert(testAdd<S<S<S<V2, V1>, V3>, V4>, V2, V1, V3, V4>(), "");
    static_assert(testAdd<S<S<S<V2, V1>, V4>, V3>, V2, V1, V4, V3>(), "");
    static_assert(testAdd<S<S<S<V2, V3>, V1>, V4>, V2, V3, V1, V4>(), "");
    static_assert(testAdd<S<S<S<V2, V3>, V4>, V1>, V2, V3, V4, V1>(), "");
    static_assert(testAdd<S<S<S<V2, V4>, V1>, V3>, V2, V4, V1, V3>(), "");
    static_assert(testAdd<S<S<S<V2, V4>, V3>, V1>, V2, V4, V3, V1>(), "");
    static_assert(testAdd<S<S<S<V3, V1>, V2>, V4>, V3, V1, V2, V4>(), "");
    static_assert(testAdd<S<S<S<V3, V1>, V4>, V2>, V3, V1, V4, V2>(), "");
    static_assert(testAdd<S<S<S<V3, V2>, V1>, V4>, V3, V2, V1, V4>(), "");
    static_assert(testAdd<S<S<S<V3, V2>, V4>, V1>, V3, V2, V4, V1>(), "");
    static_assert(testAdd<S<S<S<V3, V4>, V1>, V2>, V3, V4, V1, V2>(), "");
    static_assert(testAdd<S<S<S<V3, V4>, V2>, V1>, V3, V4, V2, V1>(), "");
    static_assert(testAdd<S<S<S<V4, V1>, V2>, V3>, V4, V1, V2, V3>(), "");
    static_assert(testAdd<S<S<S<V4, V1>, V3>, V2>, V4, V1, V3, V2>(), "");
    static_assert(testAdd<S<S<S<V4, V2>, V1>, V3>, V4, V2, V1, V3>(), "");
    static_assert(testAdd<S<S<S<V4, V2>, V3>, V1>, V4, V2, V3, V1>(), "");
    static_assert(testAdd<S<S<S<V4, V3>, V1>, V2>, V4, V3, V1, V2>(), "");
    static_assert(testAdd<S<S<S<V4, V3>, V2>, V1>, V4, V3, V2, V1>(), "");

}
