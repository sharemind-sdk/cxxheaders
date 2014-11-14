/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_APPLYTUPLES_H
#define SHAREMIND_APPLYTUPLES_H

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

template <size_t NT, size_t N, size_t ... Ns>
struct Apply {
    template<typename F, typename Tuples, typename ... Args>
    static inline auto apply(F && f, Tuples && tuples, Args && ... args)
        -> decltype(SHAREMIND_APPLYTUPLES_CALLAPPLY(N, NT, N - 1u, Ns...))
    { return SHAREMIND_APPLYTUPLES_CALLAPPLY(N, NT, N - 1u, Ns...); }
};

template <size_t NT, size_t N, size_t ... Ns>
struct Apply<NT, 1u, N, Ns...> {
    template<typename F, typename Tuples, typename ... Args>
    static inline auto apply(F && f, Tuples && tuples, Args && ... args)
        -> decltype(SHAREMIND_APPLYTUPLES_CALLAPPLY(1u, NT - 1u, N, Ns...))
    { return SHAREMIND_APPLYTUPLES_CALLAPPLY(1u, NT - 1u, N, Ns...); }
};

template <size_t NT>
struct Apply<NT, 1u> {
    template<typename F, typename Tuples, typename ... Args>
    static inline auto apply(F && f, Tuples && tuples, Args && ... args)
        -> decltype(::std::forward<F>(f)(::std::forward<Args>(args)...,
                                         SHAREMIND_APPLYTUPLES_CHOOSEELEM(1u)))
    {
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
    -> decltype(SHAREMIND_APPLYTUPLES_CALLAPPLY2)
{ return SHAREMIND_APPLYTUPLES_CALLAPPLY2; }
#undef SHAREMIND_APPLYTUPLES_CALLAPPLY2

} /* namespace Sharemind { */

#endif /* SHAREMIND_APPLYTUPLES_H */
