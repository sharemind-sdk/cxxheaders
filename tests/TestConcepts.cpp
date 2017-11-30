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

#include <type_traits>
#include <utility>


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


// Test Predicate(s):

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


// Test TrinaryPredicate:

struct TestTrinaryPredicate {
    bool operator()(int const, int *, double *&);
    static bool check(int const, int *, double *&);
    static bool check2(long const, int *, double *&);
};
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                TrinaryPredicate(T, int, int *, double *&))>
std::true_type testTrinaryPredicate(T && t);
template <typename T,
          SHAREMIND_REQUIRES_CONCEPTS(
                Not(TrinaryPredicate(T, int, int *, double *&)))>
std::false_type testTrinaryPredicate(T && t);
RETURNS_TRUE(testTrinaryPredicate(std::declval<TestTrinaryPredicate>()));
RETURNS_TRUE(testTrinaryPredicate(std::declval<TestTrinaryPredicate &>()));
RETURNS_TRUE(testTrinaryPredicate(TestTrinaryPredicate::check));
RETURNS_TRUE(testTrinaryPredicate(&TestTrinaryPredicate::check));
RETURNS_TRUE(testTrinaryPredicate(TestTrinaryPredicate::check2));
RETURNS_TRUE(testTrinaryPredicate(&TestTrinaryPredicate::check2));
RETURNS_FALSE(testTrinaryPredicate(0));


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


/** \todo Add tests for Integral, Signed, Unsigned, (Signed|Unsigned)Integral,
          NullablePointer, Hash */


int main() {}
