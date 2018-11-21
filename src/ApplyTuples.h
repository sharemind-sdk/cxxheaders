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

#ifndef SHAREMIND_APPLYTUPLES_H
#define SHAREMIND_APPLYTUPLES_H

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>
#include "DecayTupleSize.h"


namespace sharemind {
namespace Detail { namespace ApplyTuples {
#define SHAREMIND_APPLYTUPLES_CHOOSETUPLE \
    ::std::get<DecayTupleSize<Tuples>::value - NT>( \
            ::std::forward<Tuples>(tuples))
#define SHAREMIND_APPLYTUPLES_CHOOSEELEM(N) \
    ::std::get<DecayTupleSize< \
                    decltype(SHAREMIND_APPLYTUPLES_CHOOSETUPLE)>::value - N>( \
                            SHAREMIND_APPLYTUPLES_CHOOSETUPLE)
#define SHAREMIND_APPLYTUPLES_CALLAPPLY(AppendElem, ...) \
    Apply<__VA_ARGS__>::apply( \
        ::std::forward<F>(f), \
        ::std::forward<Tuples>(tuples), \
        ::std::forward<Args>(args)..., \
        SHAREMIND_APPLYTUPLES_CHOOSEELEM(AppendElem) \
    )

template <std::size_t NT, std::size_t N, std::size_t ... Ns>
struct Apply {
    template<typename F, typename Tuples, typename ... Args>
    static inline auto apply(F && f, Tuples && tuples, Args && ... args)
    { return SHAREMIND_APPLYTUPLES_CALLAPPLY(N, NT, N - 1u, Ns...); }
};

template <std::size_t NT, std::size_t N, std::size_t ... Ns>
struct Apply<NT, 1u, N, Ns...> {
    template<typename F, typename Tuples, typename ... Args>
    static inline auto apply(F && f, Tuples && tuples, Args && ... args)
    { return SHAREMIND_APPLYTUPLES_CALLAPPLY(1u, NT - 1u, N, Ns...); }
};

template <std::size_t NT>
struct Apply<NT, 1u> {
    template<typename F, typename Tuples, typename ... Args>
    static inline auto apply(F && f, Tuples && tuples, Args && ... args) {
        return ::std::forward<F>(f)(::std::forward<Args>(args)...,
                                    SHAREMIND_APPLYTUPLES_CHOOSEELEM(1u));
    }
};

#undef SHAREMIND_APPLYTUPLES_CALLAPPLY
#undef SHAREMIND_APPLYTUPLES_CHOOSEELEM
#undef SHAREMIND_APPLYTUPLES_CHOOSETUPLE

}} /* namespace Detail { namespace ApplyTuples { */

#define SHAREMIND_APPLYTUPLES_CALLAPPLY2 \
    Detail::ApplyTuples::Apply< \
        sizeof...(tpls), \
        ::std::tuple_size<typename ::std::decay<Tuples>::type>::value...>:: \
                apply(::std::forward<F>(f), \
                      ::std::forward_as_tuple(::std::forward<Tuples>(tpls)...))

template<typename F, typename ... Tuples>
inline auto applyTuples(F && f, Tuples && ... tpls)
{ return SHAREMIND_APPLYTUPLES_CALLAPPLY2; }
#undef SHAREMIND_APPLYTUPLES_CALLAPPLY2

} /* namespace Sharemind { */

#endif /* SHAREMIND_APPLYTUPLES_H */
