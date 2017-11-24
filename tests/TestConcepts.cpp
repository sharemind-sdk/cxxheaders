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


int main() {}
