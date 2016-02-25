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

#include "../src/TemplateCommonPrefixTypes.h"

#include <type_traits>
#include "../src/TemplatePrefixTypes.h"
#include "../src/TemplateAppendTypes.h"
#include "../src/TemplatePrependTypes.h"
#include "../src/TemplateInstantiateWithTypeParams.h"

using namespace sharemind;

template <typename ...> struct A;
template <typename ...> struct B;
template <typename ...> struct C;
template <typename ...> struct X;
template <typename ...> struct Y;
template <typename ...> struct Z;

struct T; struct U; struct V; struct W;

using XT = X<T>;
using YT = Y<T>;
using ZT = Z<T>;

template <typename ... Ts> using R = TemplateTypeList<Ts...>;

template <typename Result, typename ... Ts>
using Test_ = std::is_same<TemplateCommonPrefixTypes_t<Ts...>, Result>;

template <std::size_t N> struct PF {
    template <typename ... Ts> using P = TemplatePrefixTypes_t<N, Ts...>;
    template <typename ... Ts>
    using type = TemplateInstantiateWithTypeParams_t<P, Ts...>;
};

template <typename Result, typename ... Ts>
using Test_2 =
        std::integral_constant<
            bool,
            Test_<Result, Ts...>::value
            && Test_<R<>, Ts..., X<> >::value
            && Test_<R<>, Ts..., X<>, X<> >::value
            && Test_<R<>, Ts..., X<>, X<>, X<> >::value
            && Test_<R<>, Ts..., X<>, Y<> >::value
            && Test_<R<>, Ts..., X<>, Y<>, Z<> >::value
            && Test_<R<>, X<>, Ts...>::value
            && Test_<R<>, X<>, X<>, Ts...>::value
            && Test_<R<>, X<>, X<>, X<>, Ts...>::value
            && Test_<R<>, X<>, Y<>, Ts...>::value
            && Test_<R<>, X<>, Y<>, Z<>, Ts...>::value
        >;

template <std::size_t RS, typename Result, typename ... Ts> struct Test_3;

template <typename Result, typename ... Ts>
struct Test_3<1u, Result, Ts...>: Test_2<Result, Ts...> {};

template <std::size_t RS, typename Result, typename ... Ts>
struct Test_3
        : std::integral_constant<
            bool,
            Test_3<RS - 1u, Result, Ts...>::value
            &&  Test_2<
                    typename PF<RS - 1u>::template type<Result>,
                    typename PF<RS - 1u>::template type<TemplateCommonPrefixTypes_t<Ts...> >,
                    Ts...
                >::value
            &&  Test_2<
                    typename PF<RS - 1u>::template type<Result>,
                    Ts...,
                    typename PF<RS - 1u>::template type<TemplateCommonPrefixTypes_t<Ts...> >
                >::value
            &&  Test_2<
                    typename PF<RS - 1u>::template type<Result>,
                    TemplateAppendTypes_t<
                        typename PF<RS - 1u>::template type<TemplateCommonPrefixTypes_t<Ts...> >,
                        W
                    >,
                    Ts...
                >::value
            &&  Test_2<
                    typename PF<RS - 1u>::template type<Result>,
                    Ts...,
                    TemplateAppendTypes_t<
                        typename PF<RS - 1u>::template type<TemplateCommonPrefixTypes_t<Ts...> >,
                        W
                    >
                >::value
        >
{};

template <typename Result, typename ... Ts>
using Test = Test_3<Result::size, Result, Ts...>;

template <typename ... Ts> using EmptyTest =
        std::integral_constant<
            bool,
            Test_<R<>, Ts...>::value
            && (sizeof...(Ts) == 0u
                || (
                    Test_<R<>, Ts..., XT>::value
                    && Test_<R<>, Ts..., XT, XT>::value
                    && Test_<R<>, Ts..., XT, XT, XT>::value
                    && Test_<R<>, Ts..., XT, YT>::value
                    && Test_<R<>, Ts..., XT, YT, ZT>::value
                    && Test_<R<>, XT, Ts...>::value
                    && Test_<R<>, XT, XT, Ts...>::value
                    && Test_<R<>, XT, XT, XT, Ts...>::value
                    && Test_<R<>, XT, YT, Ts...>::value
                    && Test_<R<>, XT, YT, ZT, Ts...>::value
                )
            )
        >;

static_assert(EmptyTest<>::value, "");
static_assert(EmptyTest<A<> >::value, "");
static_assert(EmptyTest<A<>, A<> >::value, "");
static_assert(EmptyTest<A<>, B<> >::value, "");
static_assert(EmptyTest<A<>, A<>, A<> >::value, "");
static_assert(EmptyTest<A<>, A<>, A<>, B<> >::value, "");
static_assert(EmptyTest<B<>, A<>, A<>, A<> >::value, "");
static_assert(EmptyTest<A<>, B<>, C<> >::value, "");

static_assert(EmptyTest<A<>, A<>, B<> >::value, "");
static_assert(EmptyTest<A<>, B<>, A<> >::value, "");
static_assert(EmptyTest<A<>, B<>, B<> >::value, "");
static_assert(EmptyTest<A<>, B<>, C<> >::value, "");

static_assert(EmptyTest<A<>, A<T> >::value, "");
static_assert(EmptyTest<A<T>, A<> >::value, "");
static_assert(EmptyTest<A<>, B<T> >::value, "");
static_assert(EmptyTest<A<T>, B<> >::value, "");

static_assert(EmptyTest<A<>, A<>, A<T> >::value, "");
static_assert(EmptyTest<A<>, A<T>, A<> >::value, "");
static_assert(EmptyTest<A<>, A<T>, A<T> >::value, "");
static_assert(EmptyTest<A<T>, A<>, A<> >::value, "");
static_assert(EmptyTest<A<T>, A<>, A<T> >::value, "");
static_assert(EmptyTest<A<T>, A<T>, A<> >::value, "");

static_assert(EmptyTest<A<>, B<>, C<T> >::value, "");
static_assert(EmptyTest<A<>, B<T>, C<> >::value, "");
static_assert(EmptyTest<A<>, B<T>, C<T> >::value, "");
static_assert(EmptyTest<A<T>, B<>, C<> >::value, "");
static_assert(EmptyTest<A<T>, B<>, C<T> >::value, "");
static_assert(EmptyTest<A<T>, B<T>, C<> >::value, "");

static_assert(EmptyTest<A<T>, A<U> >::value, "");
static_assert(EmptyTest<A<T>, B<U> >::value, "");
static_assert(EmptyTest<A<T, U>, A<U> >::value, "");
static_assert(EmptyTest<A<T, U>, A<V> >::value, "");
static_assert(EmptyTest<A<T, U>, B<U> >::value, "");
static_assert(EmptyTest<A<T, U>, B<V> >::value, "");
static_assert(EmptyTest<A<T, U>, A<U, T> >::value, "");
static_assert(EmptyTest<A<T, U>, A<V, T> >::value, "");
static_assert(EmptyTest<A<T, U>, B<U, T> >::value, "");
static_assert(EmptyTest<A<T, U>, B<V, T> >::value, "");

static_assert(Test<R<T>, A<T> >::value, "");
static_assert(Test<R<T, T>, A<T, T> >::value, "");
static_assert(Test<R<T, T, T>, A<T, T, T> >::value, "");
static_assert(Test<R<T, U>, A<T, U> >::value, "");
static_assert(Test<R<T, U, V>, A<T, U, V> >::value, "");

static_assert(Test<R<T>, A<T>, A<T> >::value, "");
static_assert(Test<R<T>, A<T>, B<T> >::value, "");
static_assert(Test<R<T, T>, A<T, T>, B<T, T> >::value, "");
static_assert(Test<R<T, T, T>, A<T, T, T>, B<T, T, T> >::value, "");
static_assert(Test<R<T>, A<T>, A<T>, A<T> >::value, "");
static_assert(Test<R<T>, A<T>, B<T>, C<T> >::value, "");
static_assert(Test<R<T, T>, A<T, T>, B<T, T>, C<T, T> >::value, "");
static_assert(Test<R<T, T, T>, A<T, T, T>, B<T, T, T>, C<T, T, T> >::value, "");

static_assert(Test<R<T>, A<T, U>, A<T> >::value, "");
static_assert(Test<R<T>, A<T, U>, A<T, V> >::value, "");
static_assert(Test<R<T>, A<T>, A<T, U> >::value, "");
static_assert(Test<R<T>, A<T, V>, A<T, U> >::value, "");
static_assert(Test<R<T, U>, A<T, U>, A<T, U> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, A<T> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, A<T, V> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, A<T, V, U> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, A<T, V, V> >::value, "");
static_assert(Test<R<T>, A<T>, A<T, U, V> >::value, "");
static_assert(Test<R<T, U>, A<T, U, V>, A<T, U> >::value, "");
static_assert(Test<R<T, U>, A<T, U, V>, A<T, U, U> >::value, "");
static_assert(Test<R<T, U>, A<T, U>, A<T, U, V> >::value, "");
static_assert(Test<R<T, U>, A<T, U, U>, A<T, U, V> >::value, "");
static_assert(Test<R<T, U, V>, A<T, U, V>, A<T, U, V> >::value, "");

static_assert(Test<R<T>, A<T, U>, B<T> >::value, "");
static_assert(Test<R<T>, A<T, U>, B<T, V> >::value, "");
static_assert(Test<R<T>, A<T>, B<T, U> >::value, "");
static_assert(Test<R<T>, A<T, V>, B<T, U> >::value, "");
static_assert(Test<R<T, U>, A<T, U>, B<T, U> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, B<T> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, B<T, V> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, B<T, V, U> >::value, "");
static_assert(Test<R<T>, A<T, U, V>, B<T, V, V> >::value, "");
static_assert(Test<R<T>, A<T>, B<T, U, V> >::value, "");
static_assert(Test<R<T, U>, A<T, U, V>, B<T, U> >::value, "");
static_assert(Test<R<T, U>, A<T, U, V>, B<T, U, U> >::value, "");
static_assert(Test<R<T, U>, A<T, U>, B<T, U, V> >::value, "");
static_assert(Test<R<T, U>, A<T, U, U>, B<T, U, V> >::value, "");
static_assert(Test<R<T, U, V>, A<T, U, V>, B<T, U, V> >::value, "");


int main() {}
