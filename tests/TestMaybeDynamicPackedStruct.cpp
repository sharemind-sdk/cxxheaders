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

#include "../src/MaybeDynamicPackedStruct.h"

#include <type_traits>

using namespace sharemind;
using R = DynamicFieldPlaceholder;
template <typename T> using A = DynamicVectorFieldPlaceholder<T>;
template <typename ... Ts> using D = DynamicPackedStruct<Ts...>;
template <typename ... Ts> using M = MaybeDynamicPackedStruct<Ts...>;
template <typename ... Ts> using S = PackedStruct<Ts...>;

template <template <typename ...> class T, typename ... Ts>
constexpr bool test() noexcept
{ return std::is_same<M<Ts...>, T<Ts...> >::value; }

template <typename ... Ts> constexpr bool testStatic() noexcept
{ return test<S, Ts...>(); }

template <typename ... Ts> constexpr bool testDynamic() noexcept
{ return test<D, Ts...>(); }

int main() {
    static_assert(testStatic<>(), "");
    static_assert(testStatic<int>(), "");
    static_assert(testStatic<int, int>(), "");
    static_assert(testStatic<int, int, int>(), "");
    static_assert(testDynamic<A<int> >(), "");
    static_assert(testDynamic<R>(), "");
    static_assert(testDynamic<int, A<int> >(), "");
    static_assert(testDynamic<int, R>(), "");
    static_assert(testDynamic<A<int>, int>(), "");
    static_assert(testDynamic<R, int>(), "");
    static_assert(testDynamic<int, A<int>, int>(), "");
    static_assert(testDynamic<int, R, int>(), "");
}
