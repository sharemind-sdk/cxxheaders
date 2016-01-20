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

#ifndef SHAREMIND_NONULLTUPLE_H
#define SHAREMIND_NONULLTUPLE_H

#include <cstddef>
#include <tuple>
#include <type_traits>
#include "compiler-support/GccPR54526.h"
#include "IsNullPointer.h"
#include "TemplateCondAppendType.h"


namespace sharemind {
namespace Detail { namespace NoNullTuple {

/* noNullGet helpers: */

template <size_t I, typename Tpl> struct IndexInNonNullTuple__;
template <typename T, typename ... Ts>
struct IndexInNonNullTuple__<0u, ::std::tuple<T, Ts...> >
        : ::std::enable_if<
              !IsNullPointer<T>::value,
              ::std::integral_constant<size_t, 0u>
          >::type
{};
template <size_t I, typename T, typename ... Ts>
struct IndexInNonNullTuple__<I, ::std::tuple<T, Ts...> >
        : ::std::integral_constant<
                size_t,
                (IsNullPointer<T>::value ? 0u : 1u)
                + IndexInNonNullTuple__<I - 1u, ::std::tuple<Ts...> >::value>
{};

template <size_t I, typename Tpl>
struct IndexInNonNullTuple
        : IndexInNonNullTuple__<I, typename ::std::decay<Tpl>::type> {};


/* makeNoNullptrTuple helpers: */

template <typename Tpl, typename ... Ts>
using Filter2 = TemplateCondAppendType<IsNullPointer, std::tuple, Tpl, Ts...>;

template <typename Tpl> struct MakeNoNullTuple__ {};
template <typename ... Ts>
struct MakeNoNullTuple__<SHAREMIND_GCCPR54526_WORKAROUND::std::tuple<Ts...> >
        : Filter2<SHAREMIND_GCCPR54526_WORKAROUND::std::tuple<>, Ts...> {};

struct NoNullptrTupleExtender {

    template <typename Tpl>
    constexpr static Tpl extend(Tpl tpl)
    { return tpl; }

    template <typename Tpl, typename ... Ts>
    constexpr static typename Filter2<Tpl, Ts...>::type
    extend(Tpl tpl, decltype(nullptr), Ts && ... ts)
    { return extend(std::move(tpl), std::forward<Ts>(ts)...);}

    template <typename Tpl, typename T, typename ... Ts>
    constexpr static typename Filter2<Tpl, T, Ts...>::type
    extend(Tpl tpl, T && t, Ts && ... ts)
    {
        return NoNullptrTupleExtender::extend(
                std::tuple_cat(std::move(tpl),
                               std::make_tuple(std::forward<T>(t))),
                std::forward<Ts>(ts)...);
    }

};


/* toNoNullptrTuple helpers: */

template <typename End, size_t I = 0u> struct ToNoNullptrTuple;
template <size_t I>
struct ToNoNullptrTuple<SHAREMIND_GCCPR54526_WORKAROUND::std::tuple<>, I> {
  template <typename Start, typename Tpl>
  static constexpr Start convert(Start start, Tpl &&) { return start; }
};
template <size_t I, typename T, typename ... Ts>
struct ToNoNullptrTuple<SHAREMIND_GCCPR54526_WORKAROUND::std::tuple<T, Ts...>,
                        I>
{
    template <typename Start, typename Tpl>
    static constexpr typename Filter2<Start, T, Ts...>::type
    convert(Start start, Tpl && tpl) {
        return ToNoNullptrTuple<
                   SHAREMIND_GCCPR54526_WORKAROUND::std::tuple<Ts...>,
                   I + 1u>::convert(
              NoNullptrTupleExtender::extend(std::move(start),
                                             std::get<I>(tpl)),
              std::forward<Tpl>(tpl));
    }
};

}} /* namespace Detail { namespace NoNullTuple { */

template <typename Tpl> struct MakeNoNullTuple
        : Detail::NoNullTuple::MakeNoNullTuple__<
                typename ::std::decay<Tpl>::type> {};

template <size_t I, typename OriginalTuple, typename NoNullTuple>
constexpr auto noNullGet(NoNullTuple && t)
        -> decltype(::std::get<Detail::NoNullTuple::IndexInNonNullTuple<
                            I,
                            OriginalTuple>::value>(t))
{
    return ::std::get<
            Detail::NoNullTuple::IndexInNonNullTuple<
                    I,
                    OriginalTuple>::value>(t);
}

template <typename ... Ts>
constexpr typename MakeNoNullTuple<std::tuple<Ts...> >::type makeNoNullTuple(
        Ts && ... ts)
{
    return Detail::NoNullTuple::NoNullptrTupleExtender::extend(
                std::tuple<>{},
                std::forward<Ts>(ts)...);
}

template <typename Tpl>
constexpr typename MakeNoNullTuple<Tpl>::type toNoNullTuple(Tpl && tpl) {
    using Detail::NoNullTuple::ToNoNullptrTuple;
    return ToNoNullptrTuple<typename ::std::decay<Tpl>::type>::convert(
            ::std::tuple<>{},
            std::forward<Tpl>(tpl));
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_NONULLTUPLE_H */
