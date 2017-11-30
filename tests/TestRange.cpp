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

#include "../src/Range.h"

#include <sharemind/TestAssert.h>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef SHAREMIND_CXXHEADERS_TEST_NO_MAIN
#include "TestIterator.cpp"
#else
#define SHAREMIND_CXXHEADERS_TEST_NO_MAIN
#include "TestIterator.cpp"
#undef SHAREMIND_CXXHEADERS_TEST_NO_MAIN
#endif


// Test Range:

/// \todo Improve Range tests
template <typename T,
          typename Iterator = T *,
          typename ConstIterator = T const *>
struct TestRange {
    Iterator begin();
    ConstIterator begin() const;
    constexpr decltype(nullptr) end() const noexcept;
};
template <typename T,
          typename Iterator = T *,
          typename ConstIterator = T const *>
struct TestRangeBounded {
    Iterator begin();
    ConstIterator begin() const;
    Iterator end();
    ConstIterator end() const;
};
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Range(T))>
std::true_type testRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(Range(T)))>
std::false_type testRange(T && t);
RETURNS_TRUE(testRange("This is a fixed-length char const array"));
RETURNS_TRUE(testRange(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testRange(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testRange(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testRange(42));


// Test RangeTo:

/// \todo Improve RangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(RangeTo(T, int))>
std::true_type testRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(RangeTo(T, int)))>
std::false_type testRangeTo(T && t);
RETURNS_TRUE(testRangeTo(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testRangeTo(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testRangeTo(std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testRangeTo(42));
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(RangeTo(T, char))>
std::true_type testRangeToChar(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(RangeTo(T, char)))>
std::false_type testRangeToChar(T && t);
RETURNS_TRUE(testRangeToChar("This is a fixed-length char const array"));


// Test BoundedRange:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BoundedRange(T))>
std::true_type testBoundedRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(BoundedRange(T)))>
std::false_type testBoundedRange(T && t);
RETURNS_TRUE(testBoundedRange("This is a fixed-length char const array"));
RETURNS_FALSE(testBoundedRange(std::declval<TestRange<int> &>()));
RETURNS_FALSE(testBoundedRange(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testBoundedRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testBoundedRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testBoundedRange(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testBoundedRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testBoundedRange(42));


// Test BoundedRangeTo:

/// \todo Improve BoundedRangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BoundedRangeTo(T, int))>
std::true_type testBoundedRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(BoundedRangeTo(T, int)))>
std::false_type testBoundedRangeTo(T && t);
RETURNS_FALSE(testBoundedRangeTo(std::declval<TestRange<int> &>()));
RETURNS_FALSE(testBoundedRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testBoundedRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testBoundedRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testBoundedRangeTo(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testBoundedRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testBoundedRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testBoundedRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testBoundedRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testBoundedRangeTo(
                  std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testBoundedRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testBoundedRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testBoundedRangeTo(42));
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BoundedRangeTo(T, char))>
std::true_type testBoundedRangeToChar(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(BoundedRangeTo(T, char)))>
std::false_type testBoundedRangeToChar(T && t);
RETURNS_TRUE(testBoundedRangeToChar("This is a fixed-length char const array"));


// Test SizedRange:

/// \todo Improve Range tests
template <typename ... Ts>
struct TestSizedRange: TestRange<Ts...> {
    std::size_t size() const;
};
template <typename ... Ts>
struct TestSizedRangeBounded: TestRangeBounded<Ts...> {
    std::size_t size() const;
};
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(SizedRange(T))>
std::true_type testSizedRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(SizedRange(T)))>
std::false_type testSizedRange(T && t);
RETURNS_FALSE(testSizedRange(std::declval<TestRange<int> &>()));
RETURNS_FALSE(testSizedRange(std::declval<TestRange<int> const &>()));
RETURNS_FALSE(testSizedRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_FALSE(testSizedRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testSizedRange(std::declval<TestSizedRange<int> &>()));
RETURNS_TRUE(testSizedRange(std::declval<TestSizedRange<int> const &>()));
RETURNS_TRUE(testSizedRange(std::declval<TestSizedRangeBounded<int> &>()));
RETURNS_TRUE(testSizedRange(std::declval<TestSizedRangeBounded<int> const &>()));
RETURNS_TRUE(testSizedRange(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testSizedRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testSizedRange(42));


// Test SizedRangeTo:

/// \todo Improve RangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(SizedRangeTo(T, int))>
std::true_type testSizedRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(SizedRangeTo(T, int)))>
std::false_type testSizedRangeTo(T && t);
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRange<int> &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testSizedRangeTo(std::declval<TestSizedRange<int> &>()));
RETURNS_TRUE(testSizedRangeTo(std::declval<TestSizedRange<int> const &>()));
RETURNS_TRUE(testSizedRangeTo(std::declval<TestSizedRangeBounded<int> &>()));
RETURNS_TRUE(testSizedRangeTo(std::declval<TestSizedRangeBounded<int> const &>()));
RETURNS_TRUE(testSizedRangeTo(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testSizedRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestSizedRange<long> &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestSizedRange<long> const &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestSizedRangeBounded<long> &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<TestSizedRangeBounded<long> const &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testSizedRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testSizedRangeTo(42));


// Test InputRange:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(InputRange(T))>
std::true_type testInputRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(InputRange(T)))>
std::false_type testInputRange(T && t);
RETURNS_TRUE(testInputRange("This is a fixed-length char const array"));
RETURNS_TRUE(testInputRange(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testInputRange(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testInputRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testInputRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testInputRange(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testInputRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testInputRange(42));


// Test InputRangeTo:

/// \todo Improve InputRangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(InputRangeTo(T, int))>
std::true_type testInputRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(InputRangeTo(T, int)))>
std::false_type testInputRangeTo(T && t);
RETURNS_TRUE(testInputRangeTo(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testInputRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testInputRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testInputRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testInputRangeTo(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testInputRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testInputRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testInputRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testInputRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testInputRangeTo(
                  std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testInputRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testInputRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testInputRangeTo(42));
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(InputRangeTo(T, char))>
std::true_type testInputRangeToChar(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(InputRangeTo(T, char)))>
std::false_type testInputRangeToChar(T && t);
RETURNS_TRUE(testInputRangeToChar("This is a fixed-length char const array"));


// Test OutputRange:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(OutputRange(T))>
std::true_type testOutputRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(OutputRange(T)))>
std::false_type testOutputRange(T && t);
RETURNS_TRUE(testOutputRange(std::declval<TestRange<int> &>()));
RETURNS_FALSE(testOutputRange(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testOutputRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_FALSE(testOutputRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testOutputRange(std::declval<std::vector<int> &>()));
RETURNS_FALSE(testOutputRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testOutputRange(42));


// Test OutputRangeTo:

/// \todo Improve OutputRangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(OutputRangeTo(T, int))>
std::true_type testOutputRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(OutputRangeTo(T, int)))>
std::false_type testOutputRangeTo(T && t);
RETURNS_TRUE(testOutputRangeTo(std::declval<TestRange<int> &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testOutputRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testOutputRangeTo(std::declval<std::vector<int> &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testOutputRangeTo(
                  std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testOutputRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testOutputRangeTo(42));


// Test ForwardRange:

template <typename T>
struct TestForwardRangeIterator: TestForwardIterator<T> {
    using TestForwardIterator<T>::TestForwardIterator;
    TestForwardRangeIterator & operator++();
    TestForwardRangeIterator operator++(int);
};
template <typename T>
void swap(TestForwardRangeIterator<T> &, TestForwardRangeIterator<T> &);
template <typename T>
bool operator==(TestForwardRangeIterator<T> const &, decltype(nullptr) const &);
template <typename T>
bool operator==(decltype(nullptr) const &, TestForwardRangeIterator<T> const &);
template <typename T>
bool operator!=(TestForwardRangeIterator<T> const &, decltype(nullptr) const &);
template <typename T>
bool operator!=(decltype(nullptr) const &, TestForwardRangeIterator<T> const &);
template <typename T>
struct TestForwardRange
        : TestRange<
            T,
            TestForwardRangeIterator<T>,
            TestForwardRangeIterator<T const>
        >
{};
template <typename T>
struct TestForwardRangeBounded
        : TestRangeBounded<
            T,
            TestForwardRangeIterator<T>,
            TestForwardRangeIterator<T const>
        >
{};
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(ForwardRange(T))>
std::true_type testForwardRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(ForwardRange(T)))>
std::false_type testForwardRange(T && t);
RETURNS_TRUE(testForwardRange("This is a fixed-length char const array"));
RETURNS_TRUE(testForwardRange(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testForwardRange(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testForwardRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testForwardRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testForwardRange(std::declval<TestForwardRange<int> &>()));
RETURNS_TRUE(testForwardRange(std::declval<TestForwardRange<int> const &>()));
RETURNS_TRUE(testForwardRange(std::declval<TestForwardRangeBounded<int> &>()));
RETURNS_TRUE(testForwardRange(
                 std::declval<TestForwardRangeBounded<int> const &>()));
RETURNS_TRUE(testForwardRange(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testForwardRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testForwardRange(42));


// Test ForwardRangeTo:

/// \todo Improve ForwardRangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(ForwardRangeTo(T, int))>
std::true_type testForwardRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(ForwardRangeTo(T, int)))>
std::false_type testForwardRangeTo(T && t);
RETURNS_TRUE(testForwardRangeTo(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testForwardRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testForwardRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testForwardRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testForwardRangeTo(std::declval<TestForwardRange<int> &>()));
RETURNS_TRUE(testForwardRangeTo(std::declval<TestForwardRange<int> const &>()));
RETURNS_TRUE(testForwardRangeTo(
                 std::declval<TestForwardRangeBounded<int> &>()));
RETURNS_TRUE(testForwardRangeTo(
                 std::declval<TestForwardRangeBounded<int> const &>()));
RETURNS_TRUE(testForwardRangeTo(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testForwardRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testForwardRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testForwardRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testForwardRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testForwardRangeTo(
                  std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testForwardRangeTo(std::declval<TestForwardRange<long> &>()));
RETURNS_FALSE(testForwardRangeTo(
                  std::declval<TestForwardRange<long> const &>()));
RETURNS_FALSE(testForwardRangeTo(
                  std::declval<TestForwardRangeBounded<long> &>()));
RETURNS_FALSE(testForwardRangeTo(
                  std::declval<TestForwardRangeBounded<long> const &>()));
RETURNS_FALSE(testForwardRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testForwardRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testForwardRangeTo(42));
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(ForwardRangeTo(T, char))>
std::true_type testForwardRangeToChar(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(ForwardRangeTo(T, char)))>
std::false_type testForwardRangeToChar(T && t);
RETURNS_TRUE(testForwardRangeToChar("This is a fixed-length char const array"));


// Test BidirectionalRange:

template <typename T>
struct TestBidirectionalRangeIterator: TestBidirectionalIterator<T> {
    using TestBidirectionalIterator<T>::TestBidirectionalIterator;
    TestBidirectionalRangeIterator & operator++();
    TestBidirectionalRangeIterator operator++(int);
    TestBidirectionalRangeIterator & operator--();
    TestBidirectionalRangeIterator operator--(int);
};
template <typename T>
void swap(TestBidirectionalRangeIterator<T> &,
          TestBidirectionalRangeIterator<T> &);
template <typename T>
bool operator==(TestBidirectionalRangeIterator<T> const &,
                decltype(nullptr) const &);
template <typename T>
bool operator==(decltype(nullptr) const &,
                TestBidirectionalRangeIterator<T> const &);
template <typename T>
bool operator!=(TestBidirectionalRangeIterator<T> const &,
                decltype(nullptr) const &);
template <typename T>
bool operator!=(decltype(nullptr) const &,
                TestBidirectionalRangeIterator<T> const &);
template <typename T>
struct TestBidirectionalRange
        : TestRange<
            T,
            TestBidirectionalRangeIterator<T>,
            TestBidirectionalRangeIterator<T const>
        >
{};
template <typename T>
struct TestBidirectionalRangeBounded
        : TestRangeBounded<
            T,
            TestBidirectionalRangeIterator<T>,
            TestBidirectionalRangeIterator<T const>
        >
{};
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BidirectionalRange(T))>
std::true_type testBidirectionalRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(BidirectionalRange(T)))>
std::false_type testBidirectionalRange(T && t);
RETURNS_TRUE(testBidirectionalRange("This is a fixed-length char const array"));
RETURNS_TRUE(testBidirectionalRange(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<TestBidirectionalRange<int> &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<TestBidirectionalRange<int> const &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<TestBidirectionalRangeBounded<int> &>()));
RETURNS_TRUE(testBidirectionalRange(
                 std::declval<TestBidirectionalRangeBounded<int> const &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testBidirectionalRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testBidirectionalRange(42));


// Test BidirectionalRangeTo:

/// \todo Improve BidirectionalRangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BidirectionalRangeTo(T, int))>
std::true_type testBidirectionalRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(BidirectionalRangeTo(T, int)))>
std::false_type testBidirectionalRangeTo(T && t);
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<TestBidirectionalRange<int> &>()));
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<TestBidirectionalRange<int> const &>()));
RETURNS_TRUE(testBidirectionalRangeTo(
                 std::declval<TestBidirectionalRangeBounded<int> &>()));
RETURNS_TRUE(testBidirectionalRangeTo(
                 std::declval<TestBidirectionalRangeBounded<int> const &>()));
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testBidirectionalRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testBidirectionalRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testBidirectionalRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testBidirectionalRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testBidirectionalRangeTo(
                  std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testBidirectionalRangeTo(std::declval<TestBidirectionalRange<long> &>()));
RETURNS_FALSE(testBidirectionalRangeTo(
                  std::declval<TestBidirectionalRange<long> const &>()));
RETURNS_FALSE(testBidirectionalRangeTo(
                  std::declval<TestBidirectionalRangeBounded<long> &>()));
RETURNS_FALSE(testBidirectionalRangeTo(
                  std::declval<TestBidirectionalRangeBounded<long> const &>()));
RETURNS_FALSE(testBidirectionalRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testBidirectionalRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testBidirectionalRangeTo(42));
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(BidirectionalRangeTo(T, char))>
std::true_type testBidirectionalRangeToChar(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(Not(BidirectionalRangeTo(T, char)))>
std::false_type testBidirectionalRangeToChar(T && t);
RETURNS_TRUE(testBidirectionalRangeToChar(
                 "This is a fixed-length char const array"));


// Test RandomAccessRange:

template <typename T>
struct TestRandomAccessRangeIterator: TestRandomAccessIterator<T> {
    using TestRandomAccessIterator<T>::TestRandomAccessIterator;
    TestRandomAccessRangeIterator & operator++();
    TestRandomAccessRangeIterator operator++(int);
    TestRandomAccessRangeIterator & operator--();
    TestRandomAccessRangeIterator operator--(int);
    TestRandomAccessRangeIterator & operator+=(
            typename TestRandomAccessIterator<T>::difference_type);
    TestRandomAccessRangeIterator & operator-=(
            typename TestRandomAccessIterator<T>::difference_type);
};
template <typename T>
void swap(TestRandomAccessRangeIterator<T> &,
          TestRandomAccessRangeIterator<T> &);
template <typename T>
bool operator==(TestRandomAccessRangeIterator<T> const &,
                decltype(nullptr) const &);
template <typename T>
bool operator==(decltype(nullptr) const &,
                TestRandomAccessRangeIterator<T> const &);
template <typename T>
bool operator!=(TestRandomAccessRangeIterator<T> const &,
                decltype(nullptr) const &);
template <typename T>
bool operator!=(decltype(nullptr) const &,
                TestRandomAccessRangeIterator<T> const &);
template <typename T>
TestRandomAccessRangeIterator<T> operator+(
        TestRandomAccessRangeIterator<T> const &,
        typename TestRandomAccessRangeIterator<T>::difference_type const &);
template <typename T>
TestRandomAccessRangeIterator<T> operator+(
        typename TestRandomAccessRangeIterator<T>::difference_type const &,
        TestRandomAccessRangeIterator<T> const &);
template <typename T>
TestRandomAccessRangeIterator<T> operator-(
        TestRandomAccessRangeIterator<T> const &,
        typename TestRandomAccessRangeIterator<T>::difference_type const &);
template <typename T>
typename TestRandomAccessRangeIterator<T>::difference_type operator-(
        TestRandomAccessRangeIterator<T> const &,
        TestRandomAccessRangeIterator<T> const &);
template <typename T>
struct TestRandomAccessRange
        : TestRange<
            T,
            TestRandomAccessRangeIterator<T>,
            TestRandomAccessRangeIterator<T const>
        >
{};
template <typename T>
struct TestRandomAccessRangeBounded
        : TestRangeBounded<
            T,
            TestRandomAccessRangeIterator<T>,
            TestRandomAccessRangeIterator<T const>
        >
{};
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(RandomAccessRange(T))>
std::true_type testRandomAccessRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(RandomAccessRange(T)))>
std::false_type testRandomAccessRange(T && t);
RETURNS_TRUE(testRandomAccessRange("This is a fixed-length char const array"));
RETURNS_TRUE(testRandomAccessRange(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<TestRandomAccessRange<int> &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<TestRandomAccessRange<int> const &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<TestRandomAccessRangeBounded<int> &>()));
RETURNS_TRUE(testRandomAccessRange(
                 std::declval<TestRandomAccessRangeBounded<int> const &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testRandomAccessRange(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testRandomAccessRange(42));


// Test RandomAccessRangeTo:

/// \todo Improve RandomAccessRangeTo tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(RandomAccessRangeTo(T, int))>
std::true_type testRandomAccessRangeTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(RandomAccessRangeTo(T, int)))>
std::false_type testRandomAccessRangeTo(T && t);
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<TestRange<int> &>()));
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<TestRange<int> const &>()));
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<TestRangeBounded<int> &>()));
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<TestRangeBounded<int> const &>()));
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<TestRandomAccessRange<int> &>()));
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<TestRandomAccessRange<int> const &>()));
RETURNS_TRUE(testRandomAccessRangeTo(
                 std::declval<TestRandomAccessRangeBounded<int> &>()));
RETURNS_TRUE(testRandomAccessRangeTo(
                 std::declval<TestRandomAccessRangeBounded<int> const &>()));
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<std::vector<int> &>()));
RETURNS_TRUE(testRandomAccessRangeTo(std::declval<std::vector<int> const &>()));
RETURNS_FALSE(testRandomAccessRangeTo(std::declval<TestRange<long> &>()));
RETURNS_FALSE(testRandomAccessRangeTo(std::declval<TestRange<long> const &>()));
RETURNS_FALSE(testRandomAccessRangeTo(std::declval<TestRangeBounded<long> &>()));
RETURNS_FALSE(testRandomAccessRangeTo(
                  std::declval<TestRangeBounded<long> const &>()));
RETURNS_FALSE(testRandomAccessRangeTo(std::declval<TestRandomAccessRange<long> &>()));
RETURNS_FALSE(testRandomAccessRangeTo(
                  std::declval<TestRandomAccessRange<long> const &>()));
RETURNS_FALSE(testRandomAccessRangeTo(
                  std::declval<TestRandomAccessRangeBounded<long> &>()));
RETURNS_FALSE(testRandomAccessRangeTo(
                  std::declval<TestRandomAccessRangeBounded<long> const &>()));
RETURNS_FALSE(testRandomAccessRangeTo(std::declval<std::vector<long> &>()));
RETURNS_FALSE(testRandomAccessRangeTo(std::declval<std::vector<long> const &>()));
RETURNS_FALSE(testRandomAccessRangeTo(42));
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(RandomAccessRangeTo(T, char))>
std::true_type testRandomAccessRangeToChar(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(Not(RandomAccessRangeTo(T, char)))>
std::false_type testRandomAccessRangeToChar(T && t);
RETURNS_TRUE(testRandomAccessRangeToChar(
                 "This is a fixed-length char const array"));


// Test asLiteralRange():

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(RandomAccessRangeTo(T, char))>
std::true_type testRangeReference(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(Not(RandomAccessRangeTo(T, char)))>
std::false_type testRandomAccessRangeToChar(T && t);

static_assert(decltype(asLiteralStringRange("Test"))::size() == 4u, "");
void testAsLiteralStringRange() {
    static_assert(noexcept(asLiteralStringRange("Test")), "");
    auto const t(asLiteralStringRange("Test"));
    using T = decltype(t);
    RETURNS_TRUE(testBoundedRangeToChar(t));
    RETURNS_TRUE(testSizedRange(t));
    RETURNS_TRUE(testRandomAccessRange(t));
    static_assert(std::is_same<RangeIteratorT<T>, char const *>::value, "");
    SHAREMIND_TESTASSERT(t.size() == 4u);
    SHAREMIND_TESTASSERT(t.end() == t.begin() + 4u);

    static char const another[] = "AnotherTest";
    static_assert(sizeof(another) == 12u, "");

    static_assert(noexcept(asLiteralStringRange(another)), "");
    auto const t2(asLiteralStringRange(another));
    using T2 = decltype(t2);
    RETURNS_TRUE(testBoundedRangeToChar(t2));
    RETURNS_TRUE(testSizedRange(t2));
    RETURNS_TRUE(testRandomAccessRange(t2));
    static_assert(std::is_same<RangeIteratorT<T2>, char const *>::value, "");
    SHAREMIND_TESTASSERT(t2.size() == sizeof(another) - 1u);
    SHAREMIND_TESTASSERT(t2.begin() == another);
    SHAREMIND_TESTASSERT(t2.end() == another + (sizeof(another) - 1u));

    static char yetAnother[] = "YetAnotherTest";
    static_assert(sizeof(yetAnother) == 15u, "");
    auto const t3(asLiteralStringRange(another));
    using T3 = decltype(t3);
    RETURNS_TRUE(testBoundedRangeToChar(t3));
    RETURNS_TRUE(testSizedRange(t3));
    RETURNS_TRUE(testRandomAccessRangeToChar(t3));
    static_assert(std::is_same<RangeIteratorT<T3>, char const *>::value, "");
    SHAREMIND_TESTASSERT(t3.size() == sizeof(another) - 1u);
    SHAREMIND_TESTASSERT(t3.begin() == another);
    SHAREMIND_TESTASSERT(t3.end() == another + (sizeof(another) - 1u));

    static_assert(std::is_same<
                        IteratorValueTypeT<RangeIteratorT<T> >,
                        char
                  >::value, "");
    static_assert(std::is_same<
                        IteratorValueTypeT<RangeIteratorT<T2> >,
                        char
                  >::value, "");
    static_assert(std::is_same<
                        IteratorValueTypeT<RangeIteratorT<T3> >,
                        char
                  >::value, "");
}


int main() {
    testAsLiteralStringRange();
}
