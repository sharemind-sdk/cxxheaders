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
#include "TemplateInverseCondAppendType.h"


namespace sharemind {

template <typename ... Ts>
struct NoNullTuple {
    using type =
            TemplateInverseCondAppendType_t<
                IsNullPointer,
                std::tuple,
                std::tuple<>,
                Ts...
            >;
};

template <typename ... Ts>
using NoNullTuple_t = typename NoNullTuple<Ts...>::type;

namespace Detail {

/* noNullGet helpers: */

template <std::size_t I, typename Tpl> struct IndexInNonNullTuple_;
template <typename T, typename ... Ts>
struct IndexInNonNullTuple_<0u, std::tuple<T, Ts...> >
        : std::enable_if<
              !IsNullPointer<T>::value,
              std::integral_constant<std::size_t, 0u>
          >::type
{};
template <std::size_t I, typename T, typename ... Ts>
struct IndexInNonNullTuple_<I, std::tuple<T, Ts...> >
        : std::integral_constant<
                std::size_t,
                (IsNullPointer<T>::value ? 0u : 1u)
                + IndexInNonNullTuple_<I - 1u, std::tuple<Ts...> >::value>
{};

template <std::size_t I, typename Tpl>
struct IndexInNonNullTuple
        : IndexInNonNullTuple_<I, typename std::decay<Tpl>::type> {};


/* makeNoNullptrTuple helpers: */

template <typename ... Ts>
constexpr static std::tuple<Ts...> makeNoNullTuple_(std::tuple<Ts...> tpl)
{ return tpl; }

template <typename ... Ts, typename T, typename ... Args>
constexpr static NoNullTuple_t<typename std::decay<Ts>::type...,
                               typename std::decay<T>::type,
                               typename std::decay<Args>::type...>
makeNoNullTuple_(std::tuple<Ts...> tpl, T && t, Args && ... args);

template <typename ... Ts, typename ... Args>
constexpr static NoNullTuple_t<typename std::decay<Ts>::type...,
                               typename std::decay<Args>::type...>
makeNoNullTuple_(std::tuple<Ts...> tpl, decltype(nullptr), Args && ... args)
{ return makeNoNullTuple_(std::move(tpl), std::forward<Args>(args)...); }

template <typename ... Ts, typename T, typename ... Args>
constexpr static NoNullTuple_t<typename std::decay<Ts>::type...,
                               typename std::decay<T>::type,
                               typename std::decay<Args>::type...>
makeNoNullTuple_(std::tuple<Ts...> tpl, T && t, Args && ... args)
{
    return makeNoNullTuple_(
            std::tuple_cat(std::move(tpl),
                           std::make_tuple(std::forward<T>(t))),
            std::forward<Args>(args)...);
}

} /* namespace Detail { */

template <std::size_t I, typename OriginalTuple, typename Tpl>
constexpr auto noNullGet(Tpl && t) ->
        decltype(
            std::get<Detail::IndexInNonNullTuple<I, OriginalTuple>::value>(t))
{ return std::get<Detail::IndexInNonNullTuple<I, OriginalTuple>::value>(t); }

template <typename ... Ts>
constexpr NoNullTuple_t<typename std::decay<Ts>::type...>
makeNoNullTuple(Ts && ... ts)
{ return Detail::makeNoNullTuple_(std::tuple<>{}, std::forward<Ts>(ts)...); }

} /* namespace Sharemind { */

#endif /* SHAREMIND_NONULLTUPLE_H */
