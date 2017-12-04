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

#ifndef SHAREMIND_RANGE_H
#define SHAREMIND_RANGE_H

#include <cstddef>
#include <iterator>
#include <utility>
#include <type_traits>
#include "Concepts.h"
#include "IntegralComparisons.h"
#include "Iterator.h"
#include "Size.h"


namespace sharemind {
namespace Detail {

template <typename T>
using DecayRangeT =
        typename std::conditional<
            std::is_array<typename std::remove_reference<T>::type>::value,
            T,
            typename std::decay<T>::type
        >::type;

} /* namespace Detail { */

template <typename T>
using RangeIteratorT = decltype(std::begin(std::declval<T &>()));

template <typename T>
using RangeSentinelT = decltype(std::end(std::declval<T &>()));

SHAREMIND_DEFINE_CONCEPT(Sentinel) {
    template <typename T, typename I>
    auto check(T && t, I && i) -> SHAREMIND_REQUIRE_CONCEPTS(
                    DefaultConstructible(T),
                    CopyConstructible(T),
                    CopyAssignable(T),
                    EqualityComparable(T, I),
                    EqualityComparable(I, T),
                    InequalityComparable(T, I),
                    InequalityComparable(I, T)
                );
};

#define SHAREMIND_RANGE_H_TO(Name) \
    SHAREMIND_DEFINE_CONCEPT(Name ## To) { \
        template <typename T, typename ValueType> \
        auto check(T && t, ValueType && v) -> SHAREMIND_REQUIRE_CONCEPTS( \
                        Name(T), \
                        Same(IteratorValueTypeT<RangeIteratorT<T> >, \
                             ValueType) \
                    ); \
    }


SHAREMIND_DEFINE_CONCEPT(Range) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                Iterator(RangeIteratorT<T>),
                Sentinel(RangeSentinelT<T>, RangeIteratorT<T>)
            );
};
SHAREMIND_RANGE_H_TO(Range);

SHAREMIND_DEFINE_CONCEPT(BoundedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
                    Range(T),
                    Same(RangeIteratorT<T>, RangeSentinelT<T>)
                );
};
SHAREMIND_RANGE_H_TO(BoundedRange);

SHAREMIND_DEFINE_CONCEPT(SizedRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
        Range(T),
        ConvertibleTo(
            decltype(
                size(std::declval<
                        typename std::remove_reference<T>::type const &>())),
            typename std::iterator_traits<RangeIteratorT<T> >::difference_type)
    );
};
SHAREMIND_RANGE_H_TO(SizedRange);

#define SHAREMIND_RANGE_H_CHAIN(Prefix, Base) \
    SHAREMIND_DEFINE_CONCEPT(Prefix ## Range) { \
        template <typename T> \
        auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS( \
                        Base(T), \
                        Prefix ## Iterator(RangeIteratorT<T>) \
                    ); \
    }; \
    SHAREMIND_RANGE_H_TO(Prefix ## Range)

SHAREMIND_RANGE_H_CHAIN(Input, Range);
SHAREMIND_RANGE_H_CHAIN(Output, Range);
SHAREMIND_RANGE_H_CHAIN(Forward, InputRange);
SHAREMIND_RANGE_H_CHAIN(Bidirectional, ForwardRange);
SHAREMIND_RANGE_H_CHAIN(RandomAccess, BidirectionalRange);

SHAREMIND_DEFINE_CONCEPT(ConstantTimeMeasurableRange) {
    template <typename T>
    auto check(T && t) -> SHAREMIND_REQUIRE_CONCEPTS(
        Any(All(BoundedRange(T), RandomAccessRange(T)), SizedRange(T)));
};
SHAREMIND_RANGE_H_TO(ConstantTimeMeasurableRange);

#undef SHAREMIND_RANGE_H_CHAIN
#undef SHAREMIND_RANGE_H_TO

template <typename CharT, std::size_t N>
class LiteralStringRange {

    static_assert(N > 0u, "");
    static_assert(std::is_const<CharT>::value, "");

public: /* Methods: */

    LiteralStringRange(CharT (& begin)[N]) : m_begin(begin) {}

    CharT * begin()  const noexcept { return m_begin; }
    CharT * cbegin() const noexcept { return m_begin; }
    CharT * end()  const noexcept   { return m_begin + size(); }
    CharT * cend() const noexcept   { return m_begin + size(); }
    constexpr static std::size_t size() noexcept { return N - 1u; }

private: /* Types: */

    CharT * m_begin;

};

template <typename CharT, std::size_t N>
constexpr LiteralStringRange<CharT, N> asLiteralStringRange(
        CharT (& literal)[N]) noexcept
{ return LiteralStringRange<CharT const, N>(literal); }

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(SizedRange(T))>
auto measureRange(T && t) noexcept(noexcept(size(std::declval<T &&>())))
        -> decltype(size(std::forward<T>(t)))
{ return size(std::forward<T>(t)); }

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(SizedRange(T)),
                BoundedRange(T),
                RandomAccessRange(T))>
auto measureRange(T && t)
    noexcept(noexcept(std::end(std::declval<T &>())
                      - std::begin(std::declval<T &>())))
        -> decltype(std::end(t) - std::begin(t))
{ return std::end(t) - std::begin(t); }

template <
        typename A,
        typename B,
        SHAREMIND_REQUIRES_CONCEPTS(
            InputRange(Detail::DecayRangeT<A>),
            InputRange(Detail::DecayRangeT<B>),
            EqualityComparable(
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<A> > >,
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<B> > >),
            None(ConstantTimeMeasurableRange(Detail::DecayRangeT<A>),
                 ConstantTimeMeasurableRange(Detail::DecayRangeT<B>))
        )>
bool rangeEqual(A && a, B && b) {
    auto aIt(std::begin(a));
    auto aEnd(std::end(a));
    auto bIt(std::begin(b));
    auto bEnd(std::end(b));
    while (!static_cast<bool>(aIt == aEnd)) {
        if (static_cast<bool>(bIt == bEnd) || !static_cast<bool>(*aIt == *bIt))
            return false;
        ++aIt;
        ++bIt;
    }
    return bIt == bEnd;
}

template <
        typename A,
        typename B,
        SHAREMIND_REQUIRES_CONCEPTS(
            InputRange(Detail::DecayRangeT<A>),
            InputRange(Detail::DecayRangeT<B>),
            EqualityComparable(
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<A> > >,
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<B> > >),
            ConstantTimeMeasurableRange(Detail::DecayRangeT<A>),
            Not(ConstantTimeMeasurableRange(Detail::DecayRangeT<B>))
        )>
bool rangeEqual(A && a, B && b) {
    auto bIt(std::begin(b));
    auto bEnd(std::end(b));
    auto const s = measureRange(a);
    if (bIt == bEnd)
        return integralZero(s);
    if (integralPositive(s)) {
        using USIZE = typename std::make_unsigned<decltype(s)>::type;
        auto uSize(static_cast<USIZE>(s));
        for (auto aIt = std::begin(a); *aIt == *bIt; ++aIt) {
            if (++bIt == bEnd)
                return --uSize <= 0u;
            if (--uSize <= 0u)
                return false;
        }
    }
    return false;
}

template <
        typename A,
        typename B,
        SHAREMIND_REQUIRES_CONCEPTS(
            InputRange(Detail::DecayRangeT<A>),
            InputRange(Detail::DecayRangeT<B>),
            EqualityComparable(
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<A> > >,
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<B> > >),
            Not(ConstantTimeMeasurableRange(Detail::DecayRangeT<A>)),
            ConstantTimeMeasurableRange(Detail::DecayRangeT<B>)
        )>
bool rangeEqual(A && a, B && b) {
    auto aIt(std::begin(a));
    auto aEnd(std::end(a));
    auto const s = measureRange(b);
    if (aIt == aEnd)
        return integralZero(s);
    if (integralPositive(s)) {
        using USIZE = typename std::make_unsigned<decltype(s)>::type;
        auto uSize(static_cast<USIZE>(s));
        for (auto bIt = std::begin(b); *aIt == *bIt; ++bIt) {
            if (++aIt == aEnd)
                return --uSize <= 0u;
            if (--uSize <= 0u)
                return false;
        }
    }
    return false;
}

template <
        typename A,
        typename B,
        SHAREMIND_REQUIRES_CONCEPTS(
            InputRange(Detail::DecayRangeT<A>),
            InputRange(Detail::DecayRangeT<B>),
            EqualityComparable(
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<A> > >,
                IteratorValueTypeT<RangeIteratorT<Detail::DecayRangeT<B> > >),
            ConstantTimeMeasurableRange(Detail::DecayRangeT<A>),
            ConstantTimeMeasurableRange(Detail::DecayRangeT<B>)
        )>
bool rangeEqual(A && a, B && b) {
    auto const s = measureRange(a);
    if (integralNotEqual(s, measureRange(b)))
        return false;
    if (integralNonPositive(s))
        return integralZero(s);
    auto aIt(std::begin(a));
    auto bIt(std::begin(b));
    using USIZE = typename std::make_unsigned<decltype(s)>::type;
    auto uSize(static_cast<USIZE>(s));
    do {
        if (!static_cast<bool>(*aIt == *bIt))
            return false;
        ++aIt;
        ++bIt;
    } while (--uSize > 0u);
    return true;
}

} /* namespace Sharemind { */


#endif /* SHAREMIND_RANGE_H */
