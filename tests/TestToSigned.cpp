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

#include "../src/ToSigned.h"

#include <cstdint>
#include <type_traits>


using sharemind::toSigned;

template <typename T>
struct TestWithUnsigned {
    static_assert(std::is_unsigned<T>::value, "");
    using R = decltype(toSigned(std::declval<T>()));
    using RR = decltype(toSigned(std::declval<T &>()));
    using RCR = decltype(toSigned(std::declval<T const &>()));
    using RVR = decltype(toSigned(std::declval<T volatile &>()));
    using RCVR = decltype(toSigned(std::declval<T const volatile &>()));
    static_assert(std::is_same<R, RR>::value, "");
    static_assert(std::is_same<R, RCR>::value, "");
    static_assert(std::is_same<R, RVR>::value, "");
    static_assert(std::is_same<R, RCVR>::value, "");
    static_assert(std::is_same<R, typename std::make_signed<T>::type>::value,
                  "");
    constexpr static bool value = true;
};

template <typename T>
struct TestWithSigned {
    static_assert(std::is_signed<T>::value, "");
    using R = decltype(toSigned(std::declval<T>()));
    using RR = decltype(toSigned(std::declval<T &>()));
    using RCR = decltype(toSigned(std::declval<T const &>()));
    using RVR = decltype(toSigned(std::declval<T volatile &>()));
    using RCVR = decltype(toSigned(std::declval<T const volatile &>()));
    static_assert(std::is_same<R, T const &>::value, "");
    static_assert(std::is_same<RR, T const &>::value, "");
    static_assert(std::is_same<RCR, T const &>::value, "");
    static_assert(std::is_same<RVR, T const volatile &>::value, "");
    static_assert(std::is_same<RCVR, T const volatile &>::value, "");
    constexpr static bool value = true;
};

static_assert(TestWithUnsigned<unsigned char>::value, "");
static_assert(TestWithUnsigned<unsigned short>::value, "");
static_assert(TestWithUnsigned<unsigned>::value, "");
static_assert(TestWithUnsigned<unsigned int>::value, "");
static_assert(TestWithUnsigned<unsigned long>::value, "");
static_assert(TestWithUnsigned<unsigned long long>::value, "");
static_assert(TestWithUnsigned<uint8_t>::value, "");
static_assert(TestWithUnsigned<uint16_t>::value, "");
static_assert(TestWithUnsigned<uint32_t>::value, "");
static_assert(TestWithUnsigned<uint64_t>::value, "");

static_assert(TestWithSigned<char>::value, "");
static_assert(TestWithSigned<short>::value, "");
static_assert(TestWithSigned<int>::value, "");
static_assert(TestWithSigned<long>::value, "");
static_assert(TestWithSigned<long long>::value, "");
static_assert(TestWithSigned<int8_t>::value, "");
static_assert(TestWithSigned<int16_t>::value, "");
static_assert(TestWithSigned<int32_t>::value, "");
static_assert(TestWithSigned<int64_t>::value, "");

int main() {}
