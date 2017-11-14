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

#ifndef SHAREMIND_INDEXSEQUENCE_H
#define SHAREMIND_INDEXSEQUENCE_H

#include <cstddef>


namespace sharemind {

/**
 * The following is a poor mans work-around for C++14 integer_sequence.
 */
template <size_t... I> struct IndexSequence { using type = IndexSequence; };

namespace Detail {

template <typename, typename> struct ConcatSequence;

template <std::size_t... I1, std::size_t... I2>
struct ConcatSequence<IndexSequence<I1...>, IndexSequence<I2...> > :
    IndexSequence<I1..., (sizeof... (I1) + I2)...>::type
{};

template <std::size_t N>
struct MakeIndexSequence_ :
    ConcatSequence<
        typename MakeIndexSequence_<N / 2>::type,
        typename MakeIndexSequence_<N - (N / 2)>::type
    >::type
{};

template <> struct MakeIndexSequence_<0> : IndexSequence<> {};
template <> struct MakeIndexSequence_<1> : IndexSequence<0> {};

} /* namespace Detail { */

template <std::size_t N>
using MakeIndexSequence = typename Detail::MakeIndexSequence_<N>::type;

} /* namespace Sharemind { */

#endif /* SHAREMIND_INDEXSEQUENCE_H */
