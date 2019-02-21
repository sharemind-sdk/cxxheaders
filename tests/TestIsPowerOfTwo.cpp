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

#include "../src/IsPowerOfTwo.h"

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>
#include "../src/TestAssert.h"


using sharemind::isPowerOfTwo;

namespace {

template <typename T>
bool safeMulBy2(T & v) noexcept {
    assert(v > 0);
    if (v > std::numeric_limits<T>::max() / 2)
        return false;
    v *= 2;
    return true;
}

template <typename T>
std::vector<T> powersOfTwo() {
    static_assert(
                (std::numeric_limits<typename std::vector<T>::size_type>::max()
                 / CHAR_BIT) >= sizeof(T), "");
    std::vector<T> r;
    T i = 1;
    do {
        r.emplace_back(i);
    } while (safeMulBy2(i));
    return r;
}

#define CAST(...) static_cast<T>(__VA_ARGS__)

template <typename T, bool = std::is_signed<T>::value> struct Test;

template <typename T>
struct Test<T, true> {
    static void test_() {
        static auto const powers = powersOfTwo<T>();
        for (auto const & p : powers)
            SHAREMIND_TESTASSERT(isPowerOfTwo(p));
        static auto const expected =
                [](T v) {
                    for (auto p : powers)
                        if (p == v)
                            return true;
                    return false;
                };
        for (T i = -40; i < 40; ++i)
            SHAREMIND_TESTASSERT(isPowerOfTwo(i) == expected(i));
        for (T i = 0; i < 80; ++i) {
            auto const v = CAST(std::numeric_limits<T>::max() - i);
            SHAREMIND_TESTASSERT(isPowerOfTwo(v) == expected(v));
            auto const v2 = CAST(std::numeric_limits<T>::min() + i);
            SHAREMIND_TESTASSERT(isPowerOfTwo(v2) == expected(v2));
        }
    }
    static void test();
};

template <typename T>
struct Test<T, false> {
    static void test_() {
        static auto const powers = powersOfTwo<T>();
        for (auto const & p : powers)
            SHAREMIND_TESTASSERT(isPowerOfTwo(p));
        static auto const expected =
                [](T v) {
                    for (auto p : powers)
                        if (p == v)
                            return true;
                    return false;
                };
        for (T i = 0; i < 40; ++i) {
            SHAREMIND_TESTASSERT(isPowerOfTwo(i) == expected(i));
            auto const v = CAST(std::numeric_limits<T>::max() - i);
            SHAREMIND_TESTASSERT(isPowerOfTwo(v) == expected(v));
        }
    }

    static void test() {
        test_();
        Test<typename std::make_signed<T>::type>::test_();
    }
};

#undef CAST

template <typename T>
void Test<T, true>::test() {
    test_();
    Test<typename std::make_unsigned<T>::type>::test_();
}

template <typename T>
void test(T = T()) { Test<T>::test(); }

} // anonymous namespace

int main() {
    static_assert(!isPowerOfTwo(false), "");
    static_assert(isPowerOfTwo(true), "");
    test(char());
    test<char16_t>();
    test<char32_t>();
    test<wchar_t>();
    test<short>();
    test<int>();
    test<long>();
    test<long long>();

    Test<std::size_t>::test();
    Test<std::ptrdiff_t>::test();

    Test<std::int8_t>::test();
    Test<std::int16_t>::test();
    Test<std::int32_t>::test();
    Test<std::int64_t>::test();
    Test<std::int_least8_t>::test();
    Test<std::int_least16_t>::test();
    Test<std::int_least32_t>::test();
    Test<std::int_least64_t>::test();
    Test<std::int_fast8_t>::test();
    Test<std::int_fast16_t>::test();
    Test<std::int_fast32_t>::test();
    Test<std::int_fast64_t>::test();
    Test<std::intmax_t>::test();
    Test<std::intptr_t>::test();
    Test<std::uint8_t>::test();
    Test<std::uint16_t>::test();
    Test<std::uint32_t>::test();
    Test<std::uint64_t>::test();
    Test<std::uint_least8_t>::test();
    Test<std::uint_least16_t>::test();
    Test<std::uint_least32_t>::test();
    Test<std::uint_least64_t>::test();
    Test<std::uint_fast8_t>::test();
    Test<std::uint_fast16_t>::test();
    Test<std::uint_fast32_t>::test();
    Test<std::uint_fast64_t>::test();
    Test<std::uintmax_t>::test();
    Test<std::uintptr_t>::test();
}
