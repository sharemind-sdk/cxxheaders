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

#include "../src/Concepts.h"

#include <string>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <vector>


using namespace sharemind;

#define STATIC_TEST_TYPE(T, ...) \
    static_assert(std::is_same<T, __VA_ARGS__>::value, "");
#define STATIC_TEST_DECLTYPE(T, ...) STATIC_TEST_TYPE(T, decltype(__VA_ARGS__))
#define RETURNS_TRUE(...) STATIC_TEST_DECLTYPE(std::true_type, __VA_ARGS__)
#define RETURNS_FALSE(...) STATIC_TEST_DECLTYPE(std::false_type, __VA_ARGS__)

// Test SHAREMIND_DEFINE_CONCEPT:

SHAREMIND_DEFINE_CONCEPT(PrefixIncrementable) {
    template <typename T>
    auto check(T && t) -> decltype(++t);
};
SHAREMIND_DEFINE_CONCEPT(PostfixIncrementable) {
    template <typename T>
    auto check(T && t) -> decltype(t++);
};


// Test SHAREMIND_REQUIRES_CONCEPT:

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(PrefixIncrementable(T))>
std::true_type f(T && t);

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(PostfixIncrementable(T))>
std::false_type f(T && t);

struct TestPostfixIncrementable { void operator++(int) noexcept; };
struct TestPrefixIncrementable { void operator++() noexcept; };
RETURNS_TRUE(f(std::declval<TestPrefixIncrementable>()));
RETURNS_FALSE(f(std::declval<TestPostfixIncrementable>()));


// Tests for Callable, ValidTypes, SHAREMIND_REQUIRE(S)?(_CONCEPTS)?:

SHAREMIND_DEFINE_CONCEPT(NoexceptPrefixIncrementable) {
    template <typename T, SHAREMIND_REQUIRES_CONCEPTS(PrefixIncrementable(T))>
    auto check(T && t) -> ValidTypes<
            SHAREMIND_REQUIRE(noexcept(++t))
    >;
};
SHAREMIND_DEFINE_CONCEPT(NotNoexceptPrefixIncrementable) {
    template <typename T, SHAREMIND_REQUIRES(!noexcept(++std::declval<T>()))>
    auto check(T && t) -> ValidTypes<
            SHAREMIND_REQUIRE_CONCEPTS(PrefixIncrementable(T))
    >;
};

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(NoexceptPrefixIncrementable(T),
                                      Callable(T, float, T &, std::true_type))>
std::true_type g(T && t);

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(NotNoexceptPrefixIncrementable(T)),
          SHAREMIND_REQUIRES_CONCEPTS(Callable(T, float, T &, long double))>
std::false_type g(T && t);

struct WeirdCallable {
    template <typename ... Args>
    void operator()(float, WeirdCallable &, Args && ...);
};
struct TestNoexceptPrefixIncrementableCallable
        : TestPrefixIncrementable
        , WeirdCallable
{};
RETURNS_TRUE(g(std::declval<TestNoexceptPrefixIncrementableCallable>()));

struct NotNoexceptPrefixIncrementableCallable: WeirdCallable {
    void operator++();
};
RETURNS_FALSE(g(std::declval<NotNoexceptPrefixIncrementableCallable>()));


// Test Same:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Same(T, bool))>
std::true_type testSame(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Same(T, int))>
std::false_type testSame(T && t);
RETURNS_TRUE(testSame(true));
RETURNS_FALSE(testSame(0));


// Test Not, Same:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Same(T, int))>
std::true_type testNot(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(Same(T, int)))>
std::false_type testNot(T && t);
RETURNS_TRUE(testNot(0));
RETURNS_FALSE(testNot(true));


// Test BaseOf:

struct TestBaseOfBaseBase {};
struct TestBaseOfBase: TestBaseOfBaseBase {};
struct TestBaseOf: TestBaseOfBase {};
struct TestBaseOfDerived: TestBaseOf {};

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BaseOf(TestBaseOfBase, T))>
std::true_type testBaseOf(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(Not(BaseOf(TestBaseOfBase, T)))>
std::false_type testBaseOf(T && t);
RETURNS_TRUE(testBaseOf(std::declval<TestBaseOf>()));
RETURNS_TRUE(testBaseOf(std::declval<TestBaseOfDerived>()));
RETURNS_TRUE(testBaseOf(std::declval<TestBaseOfBase>()));
RETURNS_FALSE(testBaseOf(std::declval<TestBaseOfBaseBase>()));


// Test DerivedFrom:

template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(DerivedFrom(T, TestBaseOfBase))>
std::true_type testDerivedFrom(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(Not(DerivedFrom(T, TestBaseOfBase)))>
std::false_type testDerivedFrom(T && t);
RETURNS_TRUE(testDerivedFrom(std::declval<TestBaseOf>()));
RETURNS_TRUE(testDerivedFrom(std::declval<TestBaseOfDerived>()));
RETURNS_TRUE(testDerivedFrom(std::declval<TestBaseOfBase>()));
RETURNS_FALSE(testDerivedFrom(std::declval<TestBaseOfBaseBase>()));


// Test ConvertibleTo:

struct TestConvertibleToTarget {};
struct TestConvertibleToSource1 { operator TestConvertibleToTarget(); };
struct TestConvertibleToSource2 {};
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                ConvertibleTo(T, TestConvertibleToTarget))>
std::true_type testConvertibleTo(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(ConvertibleTo(T, TestConvertibleToTarget)))>
std::false_type testConvertibleTo(T && t);
RETURNS_TRUE(testConvertibleTo(std::declval<TestConvertibleToSource1>()));
RETURNS_FALSE(testConvertibleTo(std::declval<TestConvertibleToSource2>()));


// Test Swappable:

/// \todo Improve Swappable tests:
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Swappable(T))>
std::true_type testSwappable(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(Swappable(T)))>
std::false_type testSwappable(T && t);
RETURNS_TRUE(testSwappable(42));
struct TestSwappable { int const value; };
RETURNS_FALSE(testSwappable(TestSwappable{42}));


/// \todo Add tests for *Constructible, *Assignable and Destructible


// Test *Comparable:

#define TEST_COMPARISION_CONCEPT(Name,op) \
    struct Test ## Name ## Comparable {}; \
    struct Test ## Name ## Comparable2 { \
        bool operator op(Test ## Name ## Comparable2 const &) const; \
    }; \
    struct Test ## Name ## Comparable3 { \
        bool operator op(Test ## Name ## Comparable3 const &); \
    }; \
    struct Test ## Name ## Comparable4 { \
        bool operator op(Test ## Name ## Comparable4 const &) = delete; \
        bool operator op(Test ## Name ## Comparable4 const &) const; \
    }; \
    template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Name ## Comparable(T))> \
    std::true_type test ## Name ## Comparable(T && t); \
    template <typename T,  \
              SHAREMIND_REQUIRES_CONCEPTS(Not(Name ## Comparable(T)))> \
    std::false_type test ## Name ## Comparable(T && t); \
    RETURNS_TRUE(test ## Name ## Comparable(42)); \
    RETURNS_FALSE(test ## Name ## Comparable( \
                        std::declval<Test ## Name ## Comparable>())); \
    RETURNS_TRUE(test ## Name ## Comparable( \
                        std::declval<Test ## Name ## Comparable2>())); \
    RETURNS_FALSE(test ## Name ## Comparable( \
                        std::declval<Test ## Name ## Comparable3>())); \
    RETURNS_FALSE(test ## Name ## Comparable( \
                        std::declval<Test ## Name ## Comparable4>()));
TEST_COMPARISION_CONCEPT(Equality,==)
TEST_COMPARISION_CONCEPT(Inequality,!=)
TEST_COMPARISION_CONCEPT(LessThan,<)
TEST_COMPARISION_CONCEPT(LessOrEqual,<=)
TEST_COMPARISION_CONCEPT(GreaterThan,>)
TEST_COMPARISION_CONCEPT(GreaterOrEqual,>=)


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


// Test ForwardIterator:

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


/// \todo Add tests for NullablePointer, Hash


int main() {}
