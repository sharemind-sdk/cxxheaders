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

#include "../src/Iterator.h"

#include <type_traits>
#include <utility>
#include <vector>
#include "../src/Concepts.h"


using namespace sharemind;

#define STATIC_TEST_TYPE(T, ...) \
    static_assert(std::is_same<T, __VA_ARGS__>::value, "");
#define STATIC_TEST_DECLTYPE(T, ...) STATIC_TEST_TYPE(T, decltype(__VA_ARGS__))
#define RETURNS_TRUE(...) STATIC_TEST_DECLTYPE(std::true_type, __VA_ARGS__)
#define RETURNS_FALSE(...) STATIC_TEST_DECLTYPE(std::false_type, __VA_ARGS__)


// Test Iterator:

#ifdef _LIBCPP_VERSION
using TestIteratorTag = std::input_iterator_tag;
#else
struct TestIteratorTag {};
#endif
template <typename T>
struct TestIteratorBase {
    using iterator_category = TestIteratorTag;
    using value_type = typename std::remove_cv<T>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;
};
template <typename T>
struct TestIterator: TestIteratorBase<T> {
    using iterator_category = TestIteratorTag;
    TestIterator(TestIterator &&) = delete;
    TestIterator(TestIterator const &);
    TestIterator & operator=(TestIterator &&) = delete;
    TestIterator & operator=(TestIterator const &);
    ~TestIterator() noexcept;
    typename TestIteratorBase<T>::reference operator*();
    TestIterator & operator++();
    int const value;
};
template <typename T>
bool swap(TestIterator<T> & a, TestIterator<T> & b);

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Iterator(T))>
std::true_type testIterator(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(Iterator(T)))>
std::false_type testIterator(T && t);
RETURNS_TRUE(testIterator(std::declval<TestIterator<int> >()));
RETURNS_FALSE(testIterator(42));


// Test IteratorTo:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(IteratorTo(T, double))>
std::true_type testIteratorTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(IteratorTo(T, double)))>
std::false_type testIteratorTo(T && t);
RETURNS_TRUE(testIteratorTo(std::declval<TestIterator<double> >()));
RETURNS_FALSE(testIteratorTo(std::declval<TestIterator<float> >()));
RETURNS_FALSE(testIteratorTo(42));


// Test InputIterator:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(InputIterator(T))>
std::true_type testInputIterator(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(InputIterator(T)))>
std::false_type testInputIterator(T && t);
RETURNS_TRUE(testInputIterator(std::declval<char const *>()));
template <typename T>
struct TestInputIterator: TestIteratorBase<T> {
    using iterator_category = std::input_iterator_tag;
    TestInputIterator(TestInputIterator &&) = delete;
    TestInputIterator(TestInputIterator const &);
    TestInputIterator & operator=(TestInputIterator &&) = delete;
    TestInputIterator & operator=(TestInputIterator const &);
    ~TestInputIterator() noexcept;
    bool operator==(TestInputIterator const &) const;
    bool operator!=(TestInputIterator const &) const;
    typename TestIteratorBase<T>::reference operator*();
    TestInputIterator & operator++();
    TestInputIterator operator++(int);
    int const value;
};
template <typename T>
bool swap(TestInputIterator<T> & a, TestInputIterator<T> & b);

RETURNS_TRUE(testInputIterator(std::declval<TestInputIterator<int> >()));
RETURNS_FALSE(testInputIterator(42));


// Test OutputIterator:

template <typename T>
struct TestOutputIterator: TestIteratorBase<T> {
    using iterator_category = std::output_iterator_tag;
    TestOutputIterator(TestOutputIterator &&) = delete;
    TestOutputIterator(TestOutputIterator const &);
    TestOutputIterator & operator=(TestOutputIterator &&) = delete;
    TestOutputIterator & operator=(TestOutputIterator const &);
    ~TestOutputIterator() noexcept;
    typename TestIteratorBase<T>::reference operator*();
    TestOutputIterator & operator++();
    TestOutputIterator operator++(int);
    int const value;
};
template <typename T>
bool swap(TestOutputIterator<T> & a, TestOutputIterator<T> & b);

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(OutputIterator(T))>
std::true_type testOutputIterator(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(OutputIterator(T)))>
std::false_type testOutputIterator(T && t);
RETURNS_TRUE(testOutputIterator(std::declval<TestOutputIterator<int> >()));
RETURNS_FALSE(testOutputIterator(42));


// Test ForwardIterator:

template <typename T>
struct TestForwardIterator: TestIteratorBase<T> {
    using iterator_category = std::forward_iterator_tag;
    TestForwardIterator();
    TestForwardIterator(TestForwardIterator &&) = delete;
    TestForwardIterator(TestForwardIterator const &);
    TestForwardIterator & operator=(TestForwardIterator &&) = delete;
    TestForwardIterator & operator=(TestForwardIterator const &);
    ~TestForwardIterator() noexcept;
    bool operator==(TestForwardIterator const &) const;
    bool operator!=(TestForwardIterator const &) const;
    typename TestIteratorBase<T>::reference operator*();
    TestForwardIterator & operator++();
    TestForwardIterator operator++(int);
    int const value;
};
template <typename T>
bool swap(TestForwardIterator<T> & a, TestForwardIterator<T> & b);

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(ForwardIterator(T))>
std::true_type testForwardIterator(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(ForwardIterator(T)))>
std::false_type testForwardIterator(T && t);
RETURNS_TRUE(testOutputIterator(std::declval<TestForwardIterator<int> >()));
RETURNS_TRUE(testForwardIterator(std::declval<TestForwardIterator<int> >()));
RETURNS_FALSE(testOutputIterator(std::declval<TestForwardIterator<int const> >()));
RETURNS_TRUE(testForwardIterator(std::declval<TestForwardIterator<int const> >()));
RETURNS_FALSE(testForwardIterator(42));


// Test BidirectionalIterator:

template <typename T>
struct TestBidirectionalIterator: TestIteratorBase<T> {
    using iterator_category = std::forward_iterator_tag;
    TestBidirectionalIterator();
    TestBidirectionalIterator(TestBidirectionalIterator &&) = delete;
    TestBidirectionalIterator(TestBidirectionalIterator const &);
    TestBidirectionalIterator & operator=(TestBidirectionalIterator &&) = delete;
    TestBidirectionalIterator & operator=(TestBidirectionalIterator const &);
    ~TestBidirectionalIterator() noexcept;
    bool operator==(TestBidirectionalIterator const &) const;
    bool operator!=(TestBidirectionalIterator const &) const;
    typename TestIteratorBase<T>::reference operator*();
    TestBidirectionalIterator & operator++();
    TestBidirectionalIterator operator++(int);
    TestBidirectionalIterator & operator--();
    TestBidirectionalIterator operator--(int);
    int const value;
};
template <typename T>
bool swap(TestBidirectionalIterator<T> & a, TestBidirectionalIterator<T> & b);

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BidirectionalIterator(T))>
std::true_type testBidirectionalIterator(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(BidirectionalIterator(T)))>
std::false_type testBidirectionalIterator(T && t);
RETURNS_TRUE(testOutputIterator(std::declval<TestBidirectionalIterator<int> >()));
RETURNS_TRUE(testBidirectionalIterator(std::declval<TestBidirectionalIterator<int> >()));
RETURNS_FALSE(testOutputIterator(std::declval<TestBidirectionalIterator<int const> >()));
RETURNS_TRUE(testBidirectionalIterator(std::declval<TestBidirectionalIterator<int const> >()));
RETURNS_FALSE(testBidirectionalIterator(42));


// Test RandomAccessIterator:

template <typename T>
struct TestRandomAccessIterator: TestIteratorBase<T> {
    using iterator_category = std::forward_iterator_tag;
    TestRandomAccessIterator();
    TestRandomAccessIterator(TestRandomAccessIterator &&) = delete;
    TestRandomAccessIterator(TestRandomAccessIterator const &);
    TestRandomAccessIterator & operator=(TestRandomAccessIterator &&) = delete;
    TestRandomAccessIterator & operator=(TestRandomAccessIterator const &);
    ~TestRandomAccessIterator() noexcept;
    bool operator==(TestRandomAccessIterator const &) const;
    bool operator!=(TestRandomAccessIterator const &) const;
    bool operator<(TestRandomAccessIterator const &) const;
    bool operator>(TestRandomAccessIterator const &) const;
    bool operator<=(TestRandomAccessIterator const &) const;
    bool operator>=(TestRandomAccessIterator const &) const;
    typename TestIteratorBase<T>::reference operator*();
    TestRandomAccessIterator & operator++();
    TestRandomAccessIterator operator++(int);
    TestRandomAccessIterator & operator--();
    TestRandomAccessIterator operator--(int);
    TestRandomAccessIterator & operator+=(
            typename TestIteratorBase<T>::difference_type);
    TestRandomAccessIterator & operator-=(
            typename TestIteratorBase<T>::difference_type);
    typename TestIteratorBase<T>::reference & operator[](
            typename TestIteratorBase<T>::difference_type);
    int const value;
};
template <typename T>
TestRandomAccessIterator<T> operator+(
        TestRandomAccessIterator<T> const &,
        typename TestRandomAccessIterator<T>::difference_type);
template <typename T>
TestRandomAccessIterator<T> operator+(
        typename TestRandomAccessIterator<T>::difference_type,
        TestRandomAccessIterator<T> const &);
template <typename T>
TestRandomAccessIterator<T> operator-(
        TestRandomAccessIterator<T> const &,
        typename TestRandomAccessIterator<T>::difference_type);
template <typename T>
typename TestRandomAccessIterator<T>::difference_type operator-(
        TestRandomAccessIterator<T> const &,
        TestRandomAccessIterator<T> const &);
template <typename T>
bool swap(TestRandomAccessIterator<T> & a, TestRandomAccessIterator<T> & b);

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(RandomAccessIterator(T))>
std::true_type testRandomAccessIterator(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(RandomAccessIterator(T)))>
std::false_type testRandomAccessIterator(T && t);

RETURNS_TRUE(testOutputIterator(
                 std::declval<TestRandomAccessIterator<int> >()));
RETURNS_TRUE(testRandomAccessIterator(
                 std::declval<TestRandomAccessIterator<int> >()));
RETURNS_FALSE(testOutputIterator(
                 std::declval<TestRandomAccessIterator<int const> >()));
RETURNS_TRUE(testRandomAccessIterator(
                 std::declval<TestRandomAccessIterator<int const> >()));
RETURNS_FALSE(testRandomAccessIterator(42));


// Test ValueSwappable:

/// \todo Improve ValueSwappable tests
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(ValueSwappable(T))>
std::true_type testValueSwappable(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(ValueSwappable(T)))>
std::false_type testValueSwappable(T && t);
struct TestValueSwappable { int value; };
struct TestValueSwappable2 { int const value; };
using TestValueSwappableIt = std::vector<TestValueSwappable>::iterator;
using TestValueSwappableIt2 = std::vector<TestValueSwappable2>::iterator;
using TestValueSwappableIt3 = std::vector<TestValueSwappable>::const_iterator;
using TestValueSwappableIt4 = std::vector<TestValueSwappable2>::const_iterator;
RETURNS_TRUE(testValueSwappable(std::declval<TestValueSwappableIt>()));
RETURNS_FALSE(testValueSwappable(std::declval<TestValueSwappableIt2>()));
RETURNS_FALSE(testValueSwappable(std::declval<TestValueSwappableIt3>()));
RETURNS_FALSE(testValueSwappable(std::declval<TestValueSwappableIt4>()));
RETURNS_FALSE(testValueSwappable(42));

#ifndef SHAREMIND_CXXHEADERS_TEST_NO_MAIN
int main() {}
#endif
