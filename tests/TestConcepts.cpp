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


// Test UnaryPredicate:

struct TestUnaryPredicate {
    bool operator()(int const);
    static bool check(int const);
    static bool check2(long const);
};
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(UnaryPredicate(T, int))>
std::true_type testUnaryPredicate(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(UnaryPredicate(T, int)))>
std::false_type testUnaryPredicate(T && t);
RETURNS_TRUE(testUnaryPredicate(std::declval<TestUnaryPredicate>()));
RETURNS_TRUE(testUnaryPredicate(std::declval<TestUnaryPredicate &>()));
RETURNS_TRUE(testUnaryPredicate(TestUnaryPredicate::check));
RETURNS_TRUE(testUnaryPredicate(&TestUnaryPredicate::check));
RETURNS_TRUE(testUnaryPredicate(TestUnaryPredicate::check2));
RETURNS_TRUE(testUnaryPredicate(&TestUnaryPredicate::check2));
RETURNS_FALSE(testUnaryPredicate(0));


// Test BinaryPredicate:

struct TestBinaryPredicate {
    bool operator()(int const, int *);
    static bool check(int const, int *);
    static bool check2(long const, int *);
};
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(BinaryPredicate(T, int, int *))>
std::true_type testBinaryPredicate(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(Not(BinaryPredicate(T, int, int *)))>
std::false_type testBinaryPredicate(T && t);
RETURNS_TRUE(testBinaryPredicate(std::declval<TestBinaryPredicate>()));
RETURNS_TRUE(testBinaryPredicate(std::declval<TestBinaryPredicate &>()));
RETURNS_TRUE(testBinaryPredicate(TestBinaryPredicate::check));
RETURNS_TRUE(testBinaryPredicate(&TestBinaryPredicate::check));
RETURNS_TRUE(testBinaryPredicate(TestBinaryPredicate::check2));
RETURNS_TRUE(testBinaryPredicate(&TestBinaryPredicate::check2));
RETURNS_FALSE(testBinaryPredicate(0));


// Test Same:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Same(T, bool))>
std::true_type testSame(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Same(T, int))>
std::false_type testSame(T && t);
RETURNS_TRUE(testSame(true));
RETURNS_FALSE(testSame(0));


// Test Not:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Same(T, int))>
std::true_type testNot(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(Same(T, int)))>
std::false_type testNot(T && t);
RETURNS_TRUE(testNot(0));
RETURNS_FALSE(testNot(true));


// Test All:

template <typename A, typename B, typename C,
          SHAREMIND_REQUIRES_CONCEPTS(All(Same(A, int),
                                          Same(B, int),
                                          Same(C, int)))>
std::true_type testAll(A && a, B && b, C && c);
template <typename A, typename B, typename C,
          SHAREMIND_REQUIRES_CONCEPTS(Not(All(Same(A, int),
                                              Same(B, int),
                                              Same(C, int))))>
std::false_type testAll(A && a, B && b, C && c);
RETURNS_TRUE(testAll(0, 0, 0));
RETURNS_FALSE(testAll(0, 0, 0l));
RETURNS_FALSE(testAll(0, 0l, 0));
RETURNS_FALSE(testAll(0, 0l, 0l));
RETURNS_FALSE(testAll(0l, 0, 0));
RETURNS_FALSE(testAll(0l, 0, 0l));
RETURNS_FALSE(testAll(0l, 0l, 0));
RETURNS_FALSE(testAll(0l, 0l, 0l));


// Test Any:

template <typename A, typename B, typename C,
          SHAREMIND_REQUIRES_CONCEPTS(Any(Same(A, int),
                                          Same(B, int),
                                          Same(C, int)))>
std::true_type testAny(A && a, B && b, C && c);
template <typename A, typename B, typename C,
          SHAREMIND_REQUIRES_CONCEPTS(Not(Any(Same(A, int),
                                              Same(B, int),
                                              Same(C, int))))>
std::false_type testAny(A && a, B && b, C && c);
RETURNS_TRUE(testAny(0, 0, 0));
RETURNS_TRUE(testAny(0, 0, 0l));
RETURNS_TRUE(testAny(0, 0l, 0));
RETURNS_TRUE(testAny(0, 0l, 0l));
RETURNS_TRUE(testAny(0l, 0, 0));
RETURNS_TRUE(testAny(0l, 0, 0l));
RETURNS_TRUE(testAny(0l, 0l, 0));
RETURNS_FALSE(testAny(0l, 0l, 0l));


// Test None:

template <typename A, typename B, typename C,
          SHAREMIND_REQUIRES_CONCEPTS(None(Same(A, int),
                                           Same(B, int),
                                           Same(C, int)))>
std::true_type testNone(A && a, B && b, C && c);
template <typename A, typename B, typename C,
          SHAREMIND_REQUIRES_CONCEPTS(Not(None(Same(A, int),
                                               Same(B, int),
                                               Same(C, int))))>
std::false_type testNone(A && a, B && b, C && c);
RETURNS_FALSE(testNone(0, 0, 0));
RETURNS_FALSE(testNone(0, 0, 0l));
RETURNS_FALSE(testNone(0, 0l, 0));
RETURNS_FALSE(testNone(0, 0l, 0l));
RETURNS_FALSE(testNone(0l, 0, 0));
RETURNS_FALSE(testNone(0l, 0, 0l));
RETURNS_FALSE(testNone(0l, 0l, 0));
RETURNS_TRUE(testNone(0l, 0l, 0l));


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


// Test DecaysTo:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(DecaysTo(T, int))>
std::true_type testDecaysTo(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(DecaysTo(T, int)))>
std::false_type testDecaysTo(T && t);
RETURNS_TRUE(testDecaysTo(std::declval<int>()));
RETURNS_TRUE(testDecaysTo(std::declval<int const>()));
RETURNS_TRUE(testDecaysTo(std::declval<int &>()));
RETURNS_TRUE(testDecaysTo(std::declval<int const &>()));
RETURNS_TRUE(testDecaysTo(std::declval<int &&>()));
RETURNS_TRUE(testDecaysTo(std::declval<int const &&>()));
RETURNS_FALSE(testDecaysTo(std::declval<char>()));


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
    struct Test ## Name ## ComparableB2 { \
        bool operator op(Test ## Name ## Comparable const &) const; \
    }; \
    struct Test ## Name ## ComparableB3 { \
        bool operator op(Test ## Name ## Comparable const &); \
    }; \
    struct Test ## Name ## ComparableB4 { \
        bool operator op(Test ## Name ## Comparable const &) = delete; \
        bool operator op(Test ## Name ## Comparable const &) const; \
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
                        std::declval<Test ## Name ## Comparable4>())); \
    template <typename A, \
              typename B, \
              SHAREMIND_REQUIRES_CONCEPTS(Name ## Comparable(A, B))> \
    std::true_type test ## Name ## Comparable2(A && a, B && b); \
    template <typename A, \
              typename B, \
              SHAREMIND_REQUIRES_CONCEPTS(Not(Name ## Comparable(A, B)))> \
    std::false_type test ## Name ## Comparable2(A && a, B && b); \
    RETURNS_TRUE(test ## Name ## Comparable2(42, 123.0)); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        42, \
                        std::declval<Test ## Name ## Comparable>())); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## Comparable>(), \
                        42)); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## Comparable>(), \
                        std::declval<Test ## Name ## Comparable>())); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## Comparable>(), \
                        std::declval<Test ## Name ## ComparableB2>())); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## Comparable>(), \
                        std::declval<Test ## Name ## ComparableB3>())); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## Comparable>(), \
                        std::declval<Test ## Name ## ComparableB4>())); \
    RETURNS_TRUE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## ComparableB2>(), \
                        std::declval<Test ## Name ## Comparable>())); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## ComparableB3>(), \
                        std::declval<Test ## Name ## Comparable>())); \
    RETURNS_FALSE(test ## Name ## Comparable2( \
                        std::declval<Test ## Name ## ComparableB4>(), \
                        std::declval<Test ## Name ## Comparable>()));
TEST_COMPARISION_CONCEPT(Equality,==)
TEST_COMPARISION_CONCEPT(Inequality,!=)
TEST_COMPARISION_CONCEPT(LessThan,<)
TEST_COMPARISION_CONCEPT(LessOrEqual,<=)
TEST_COMPARISION_CONCEPT(GreaterThan,>)
TEST_COMPARISION_CONCEPT(GreaterOrEqual,>=)


/// \todo Add tests for Integral, Signed, Unsigned, (Signed|Unsigned)Integral


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


// Test BoundedRange:

template <typename T, SHAREMIND_REQUIRES_CONCEPTS(BoundedRange(T))>
std::true_type testBoundedRange(T && t);
template <typename T, SHAREMIND_REQUIRES_CONCEPTS(Not(BoundedRange(T)))>
std::false_type testBoundedRange(T && t);
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
