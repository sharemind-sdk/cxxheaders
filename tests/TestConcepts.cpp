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

// Test SHAREMIND_DEFINE_CONCEPT:

SHAREMIND_DEFINE_CONCEPT(IsPrefixIncrementable) {
    template <typename T>
    auto check(T && t) -> decltype(++t);
};
SHAREMIND_DEFINE_CONCEPT(IsPostfixIncrementable) {
    template <typename T>
    auto check(T && t) -> decltype(t++);
};


// Test SHAREMIND_REQUIRES_CONCEPT:

struct Prefix {};
struct Postfix {};

template <typename T,
          SHAREMIND_REQUIRES_CONCEPT(IsPrefixIncrementable(T))>
Prefix f(T && t);

template <typename T,
          SHAREMIND_REQUIRES_CONCEPT(IsPostfixIncrementable(T))>
Postfix f(T && t);

struct PostfixInc { void operator++(int) noexcept; };
struct PrefixInc { void operator++() noexcept; };

static_assert(std::is_same<Prefix,
                           decltype(f(std::declval<PrefixInc>()))>::value, "");

static_assert(std::is_same<Postfix,
                           decltype(f(std::declval<PostfixInc>()))>::value, "");


/* Tests for ValidTypes, SHAREMIND_REQUIRE, SHAREMIND_REQUIRE_CONCEPT,
   SHAREMIND_REQUIRES, SHAREMIND_REQUIRES_CONCEPT: */

SHAREMIND_DEFINE_CONCEPT(IsNoexceptPrefixIncrementable) {
    template <typename T, SHAREMIND_REQUIRES_CONCEPT(IsPrefixIncrementable(T))>
    auto check(T && t) -> ValidTypes<
            SHAREMIND_REQUIRE(noexcept(++t))
    >;
};
SHAREMIND_DEFINE_CONCEPT(IsNotNoexceptPrefixIncrementable) {
    template <typename T, SHAREMIND_REQUIRES(!noexcept(++std::declval<T>()))>
    auto check(T && t) -> ValidTypes<
            SHAREMIND_REQUIRE_CONCEPT(IsPrefixIncrementable(T))
    >;
};

struct Noexcept {};
struct NotNoexcept {};

template <typename T,
          SHAREMIND_REQUIRES_CONCEPT(IsNoexceptPrefixIncrementable(T))>
Noexcept g(T && t);

template <typename T,
          SHAREMIND_REQUIRES_CONCEPT(IsNotNoexceptPrefixIncrementable(T))>
NotNoexcept g(T && t);

static_assert(std::is_same<Noexcept,
                           decltype(g(std::declval<PrefixInc>()))>::value, "");

struct PrefixInc2 { void operator++(); };
static_assert(std::is_same<NotNoexcept,
                           decltype(g(std::declval<PrefixInc2>()))>::value, "");


int main() {}
