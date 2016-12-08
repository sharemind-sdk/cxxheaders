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

#ifndef SHAREMIND_UNPACKSHAREDTUPLE_H
#define SHAREMIND_UNPACKSHAREDTUPLE_H

#include <cassert>
#include <cstddef>
#include <memory>
#include <tuple>
#include "TemplateTypeList.h"


namespace sharemind {
namespace Detail {
namespace UnpackSharedTuple {

/* A simple typesystem-level pair for type and index: */
template <typename T, std::size_t I>
struct TypeIndexPair {
    using type = T;
    constexpr static std::size_t const index = I;
};

/* TypeIndexPair sequence generator: */
template <typename Accum, typename ... Ts> struct Seq;
template <typename ... Pairs> struct Seq<TemplateTypeList<Pairs...> > {
    static std::tuple<std::shared_ptr<typename Pairs::type>...> create(
            std::shared_ptr<std::tuple<typename Pairs::type...> > const & ptr)
            noexcept
    {
        return std::tuple<std::shared_ptr<typename Pairs::type>...>{
            std::shared_ptr<typename Pairs::type>(
                        ptr,
                        &std::get<Pairs::index>(*ptr))...};
    }
};
template <typename ... AccumTs, typename T, typename ... Ts>
struct Seq<TemplateTypeList<AccumTs...>, T, Ts...>
    : Seq<
        TemplateTypeList<
            TypeIndexPair<typename AccumTs::type, AccumTs::index>...,
            TypeIndexPair<T, sizeof...(AccumTs)>
        >,
        Ts...>
{};

} /* namespace Detail { */
} /* namespace UnpackSharedTuple { */

template <typename ... Args>
std::tuple<std::shared_ptr<Args>...> unpackSharedTuple(
        std::shared_ptr<std::tuple<Args...> > const & tuple) noexcept
{
    assert(tuple);
    return Detail::UnpackSharedTuple::Seq<TemplateTypeList<>, Args...>::create(
                tuple);
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_UNPACKSHAREDTUPLE_H */
