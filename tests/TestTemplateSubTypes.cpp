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

#include "../src/TemplateSubTypes.h"

#include <cstddef>
#include <type_traits>
#include "../src/TemplateInstantiateWithTypeParams.h"
#include "../src/TemplateTypeList.h"


template <typename ...> struct I {};

struct X; struct Y; struct Z; struct T; struct U; struct V;

template <typename Input, std::size_t Index, std::size_t Size, typename ... Ts>
struct Test {

    template <typename Input_,
              std::size_t Index_,
              std::size_t Size_,
              typename ... Ts_>
    struct B {
        template <typename ... Ts2_>
        using S = sharemind::TemplateSubTypes_t<Index_, Size_, Ts2_...>;

        constexpr static bool value =
                std::is_same<
                    sharemind::TemplateInstantiateWithTypeParams_t<S, Input_>,
                    sharemind::TemplateTypeList<Ts_...>
                >::value;

    };

    template <typename P, typename S>
    using More = sharemind::TemplateInstantiateWithTypeParams_t<I, P, Input, S>;

    constexpr static bool value =
            B<Input, Index, Size, Ts...>::value
            && B<More<I<X, X, X>, I<> >, Index + 3u, Size, Ts...>::value
            && B<More<I<X, Y, Z>, I<> >, Index + 3u, Size, Ts...>::value
            && B<More<I<>, I<X, X, X> >, Index, Size, Ts...>::value
            && B<More<I<>, I<X, Y, Z> >, Index, Size, Ts...>::value
            && B<More<I<X, X, X>, I<T, T, T> >, Index + 3u, Size, Ts...>::value
            && B<More<I<X, X, X>, I<T, U, V> >, Index + 3u, Size, Ts...>::value
            && B<More<I<X, Y, Z>, I<T, T, T> >, Index + 3u, Size, Ts...>::value
            && B<More<I<X, Y, Z>, I<T, U, V> >, Index + 3u, Size, Ts...>::value;

};

struct A; struct B; struct C; struct D; struct E; struct F;

static_assert(Test<I<>, 0u, 0u>::value, "");
static_assert(Test<I<A>, 0u, 0u>::value, "");
static_assert(Test<I<A>, 1u, 0u>::value, "");
static_assert(Test<I<A, A>, 0u, 0u>::value, "");
static_assert(Test<I<A, A>, 1u, 0u>::value, "");
static_assert(Test<I<A, A>, 2u, 0u>::value, "");
static_assert(Test<I<A, B>, 0u, 0u>::value, "");
static_assert(Test<I<A, B>, 1u, 0u>::value, "");
static_assert(Test<I<A, B>, 2u, 0u>::value, "");
static_assert(Test<I<A, A, A>, 0u, 0u>::value, "");
static_assert(Test<I<A, A, A>, 1u, 0u>::value, "");
static_assert(Test<I<A, A, A>, 2u, 0u>::value, "");
static_assert(Test<I<A, A, A>, 3u, 0u>::value, "");
static_assert(Test<I<A, B, C>, 0u, 0u>::value, "");
static_assert(Test<I<A, B, C>, 1u, 0u>::value, "");
static_assert(Test<I<A, B, C>, 2u, 0u>::value, "");
static_assert(Test<I<A, B, C>, 3u, 0u>::value, "");
static_assert(Test<I<A, A, A, A>, 0u, 0u>::value, "");
static_assert(Test<I<A, A, A, A>, 1u, 0u>::value, "");
static_assert(Test<I<A, A, A, A>, 2u, 0u>::value, "");
static_assert(Test<I<A, A, A, A>, 3u, 0u>::value, "");
static_assert(Test<I<A, A, A, A>, 4u, 0u>::value, "");
static_assert(Test<I<A, B, C, D>, 0u, 0u>::value, "");
static_assert(Test<I<A, B, C, D>, 1u, 0u>::value, "");
static_assert(Test<I<A, B, C, D>, 2u, 0u>::value, "");
static_assert(Test<I<A, B, C, D>, 3u, 0u>::value, "");
static_assert(Test<I<A, B, C, D>, 4u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A>, 0u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A>, 1u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A>, 2u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A>, 3u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A>, 4u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A>, 5u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E>, 0u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E>, 1u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E>, 2u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E>, 3u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E>, 4u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E>, 5u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A, A>, 0u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A, A>, 1u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A, A>, 2u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A, A>, 3u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A, A>, 4u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A, A>, 5u, 0u>::value, "");
static_assert(Test<I<A, A, A, A, A, A>, 6u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 0u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 1u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 2u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 3u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 4u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 5u, 0u>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 6u, 0u>::value, "");

static_assert(Test<I<A>, 0u, 1u, A>::value, "");

static_assert(Test<I<A, B>, 0u, 1u, A>::value, "");
static_assert(Test<I<A, B>, 1u, 1u, B>::value, "");
static_assert(Test<I<A, B>, 0u, 2u, A, B>::value, "");

static_assert(Test<I<A, B, C>, 0u, 1u, A>::value, "");
static_assert(Test<I<A, B, C>, 1u, 1u, B>::value, "");
static_assert(Test<I<A, B, C>, 2u, 1u, C>::value, "");
static_assert(Test<I<A, B, C>, 0u, 2u, A, B>::value, "");
static_assert(Test<I<A, B, C>, 1u, 2u, B, C>::value, "");
static_assert(Test<I<A, B, C>, 0u, 3u, A, B, C>::value, "");

static_assert(Test<I<A, B, C, D>, 0u, 1u, A>::value, "");
static_assert(Test<I<A, B, C, D>, 1u, 1u, B>::value, "");
static_assert(Test<I<A, B, C, D>, 2u, 1u, C>::value, "");
static_assert(Test<I<A, B, C, D>, 3u, 1u, D>::value, "");
static_assert(Test<I<A, B, C, D>, 0u, 2u, A, B>::value, "");
static_assert(Test<I<A, B, C, D>, 1u, 2u, B, C>::value, "");
static_assert(Test<I<A, B, C, D>, 2u, 2u, C, D>::value, "");
static_assert(Test<I<A, B, C, D>, 0u, 3u, A, B, C>::value, "");
static_assert(Test<I<A, B, C, D>, 1u, 3u, B, C, D>::value, "");
static_assert(Test<I<A, B, C, D>, 0u, 4u, A, B, C, D>::value, "");


static_assert(Test<I<A, B, C, D, E>, 0u, 1u, A>::value, "");
static_assert(Test<I<A, B, C, D, E>, 1u, 1u, B>::value, "");
static_assert(Test<I<A, B, C, D, E>, 2u, 1u, C>::value, "");
static_assert(Test<I<A, B, C, D, E>, 3u, 1u, D>::value, "");
static_assert(Test<I<A, B, C, D, E>, 4u, 1u, E>::value, "");
static_assert(Test<I<A, B, C, D, E>, 0u, 2u, A, B>::value, "");
static_assert(Test<I<A, B, C, D, E>, 1u, 2u, B, C>::value, "");
static_assert(Test<I<A, B, C, D, E>, 2u, 2u, C, D>::value, "");
static_assert(Test<I<A, B, C, D, E>, 3u, 2u, D, E>::value, "");
static_assert(Test<I<A, B, C, D, E>, 0u, 3u, A, B, C>::value, "");
static_assert(Test<I<A, B, C, D, E>, 1u, 3u, B, C, D>::value, "");
static_assert(Test<I<A, B, C, D, E>, 2u, 3u, C, D, E>::value, "");
static_assert(Test<I<A, B, C, D, E>, 0u, 4u, A, B, C, D>::value, "");
static_assert(Test<I<A, B, C, D, E>, 1u, 4u, B, C, D, E>::value, "");
static_assert(Test<I<A, B, C, D, E>, 0u, 5u, A, B, C, D, E>::value, "");

static_assert(Test<I<A, B, C, D, E, F>, 0u, 1u, A>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 1u, 1u, B>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 2u, 1u, C>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 3u, 1u, D>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 4u, 1u, E>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 5u, 1u, F>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 0u, 2u, A, B>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 1u, 2u, B, C>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 2u, 2u, C, D>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 3u, 2u, D, E>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 4u, 2u, E, F>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 0u, 3u, A, B, C>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 1u, 3u, B, C, D>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 2u, 3u, C, D, E>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 3u, 3u, D, E, F>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 0u, 4u, A, B, C, D>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 1u, 4u, B, C, D, E>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 2u, 4u, C, D, E, F>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 0u, 5u, A, B, C, D, E>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 1u, 5u, B, C, D, E, F>::value, "");
static_assert(Test<I<A, B, C, D, E, F>, 0u, 6u, A, B, C, D, E, F>::value, "");

int main() {}
