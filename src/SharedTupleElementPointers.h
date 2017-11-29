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

#ifndef SHAREMIND_SHAREDTUPLEELEMENTPTRS_H
#define SHAREMIND_SHAREDTUPLEELEMENTPTRS_H

#include <cassert>
#include <cstddef>
#include <memory>
#include <tuple>


namespace sharemind {

template <std::size_t I, typename ... Args>
auto makeSharedTupleElementPtr(
        std::shared_ptr<std::tuple<Args...> > const & value) noexcept
    -> std::shared_ptr<
        typename std::tuple_element<I, std::tuple<Args...> >::type>
{
    assert(value);
    return std::shared_ptr<
            typename std::tuple_element<I, std::tuple<Args...> >::type
           >(value, &std::get<I>(*value));
}

template <std::size_t ... Is, typename ... Args>
auto makeSharedTupleElementPtrs(
        std::shared_ptr<std::tuple<Args...> > const & value) noexcept
    -> std::tuple<decltype(makeSharedTupleElementPtr<Is>(value))...>
{
    assert(value);
    return std::tuple<decltype(makeSharedTupleElementPtr<Is>(value))...>{
                makeSharedTupleElementPtr<Is>(value)...};
}

namespace Detail {
namespace UnpackSharedTuple {

/* A simple typesystem-level pair for type and index: */
template <typename T, std::size_t I>
struct TypeIndexPair: std::integral_constant<std::size_t, I>
{ using ElemType = T; };

/* TypeIndexPair sequence generator: */
template <typename AccumTs, typename ... Ts> struct Seq;

template <typename ... Pairs>
struct Seq<std::tuple<Pairs...> > {
    using R = std::tuple<std::shared_ptr<typename Pairs::ElemType>...>;

    template <typename Ptr>
    static R create(Ptr const & ptr) noexcept
    { return R{makeSharedTupleElementPtr<Pairs::value>(ptr)...}; }
};

template <typename ... AccumTs, typename T, typename ... Ts>
struct Seq<std::tuple<AccumTs...>, T, Ts...>
    : Seq<
        std::tuple<
            TypeIndexPair<typename AccumTs::ElemType, AccumTs::value>...,
            TypeIndexPair<T, sizeof...(AccumTs)>
        >,
        Ts...
      >
{};

} /* namespace Detail { */
} /* namespace UnpackSharedTuple { */

template <typename ... Args>
std::tuple<std::shared_ptr<Args>...> makeAllSharedTupleElementPtrs(
        std::shared_ptr<std::tuple<Args...> > const & value) noexcept
{
    assert(value);
    return Detail::UnpackSharedTuple::Seq<std::tuple<>, Args...>::create(value);
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_SHAREDTUPLEELEMENTPTRS_H */
