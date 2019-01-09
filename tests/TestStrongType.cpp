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

#define SHAREMIND_STRONGTYPE_H_TEST
#include "../src/StrongType.h"

#include <cstdint>
#include <ostream>
#include <type_traits>
#include <utility>


#define SA(...) static_assert(__VA_ARGS__, "")

template <typename T, typename STI>
struct TestCommonInterface: std::true_type {
    SA(std::is_same<typename STI::ValueType, T>::value);
    SA(std::is_nothrow_default_constructible<STI>::value);
    SA(std::is_nothrow_constructible<STI,
                                     typename STI::ValueType const &>::value);
    SA(std::is_nothrow_copy_constructible<STI>::value);
    SA(std::is_nothrow_move_constructible<STI>::value);
    SA(std::is_nothrow_constructible<STI, T &>::value);
    SA(std::is_nothrow_constructible<STI, T const &>::value);
    SA(std::is_nothrow_copy_assignable<STI>::value);
    SA(std::is_same<
            decltype(std::declval<STI &>() = std::declval<STI const &>()),
            STI &>::value);
    SA(std::is_nothrow_move_assignable<STI>::value);
    SA(std::is_same<decltype(std::declval<STI &>() = std::declval<STI &&>()),
                    STI &>::value);
    SA(std::is_nothrow_destructible<STI>::value);
    SA(std::is_same<decltype(std::declval<STI &>().get()),
                    typename STI::ValueType &>::value);
    SA(noexcept(std::declval<STI &>().get()));
    SA(std::is_same<decltype(std::declval<STI const &>().get()),
                    typename STI::ValueType const &>::value);
    SA(noexcept(std::declval<STI const &>().get()));
}; // template <typename STI> struct TestCommonInterface

template <typename STI>
struct TestHashableInterface: std::true_type {
    SA(std::is_same<
            decltype(std::declval<STI const &>().hash()),
            decltype(std::hash<typename STI::ValueType>()(
                         std::declval<typename STI::ValueType>()))>::value);
    SA(noexcept(std::declval<STI const &>().hash()));
}; // template <typename STI> struct TestHashableInterface

template <typename STI>
struct TestComparableInterfaces: std::true_type {
    #define TEST_IFACE(op) \
        SA(std::is_same<decltype(std::declval<STI const &>() \
                                 op std::declval<STI const &>()), \
                        bool>::value); \
        SA(noexcept(std::declval<STI const &>() \
                    op std::declval<STI const &>()));
    TEST_IFACE(==)
    TEST_IFACE(!=)
    TEST_IFACE(<)
    TEST_IFACE(<=)
    TEST_IFACE(>=)
    TEST_IFACE(>)
    #undef TEST_IFACE
}; // template <typename STI> struct TestComparableInterfaces

#define TEST_IFACE(op) \
    SA(std::is_same<decltype(std::declval<STI &>() \
                             op std::declval<STI const &>()), \
                    STI &>::value); \
    SA(noexcept(std::declval<STI &>() op std::declval<STI const &>())); \
    SA(std::is_same< \
            decltype(std::declval<STI &>() \
                     op std::declval<typename STI::ValueType const &>()), \
            STI &>::value); \
    SA(noexcept(std::declval<STI &>() \
                op std::declval<typename STI::ValueType const &>()));
template <typename STI,
          bool = std::is_floating_point<typename STI::ValueType>::value>
struct TestAssignableInterfaces: std::true_type {
    TEST_IFACE(+=)
    TEST_IFACE(-=)
    TEST_IFACE(*=)
    TEST_IFACE(/=)
    TEST_IFACE(%=)
    TEST_IFACE(^=)
    TEST_IFACE(&=)
    TEST_IFACE(|=)
}; // template <typename STI, bool> struct TestAssignableInterfaces

template <typename STI>
struct TestAssignableInterfaces<STI, true>: std::true_type {
    TEST_IFACE(+=)
    TEST_IFACE(-=)
    TEST_IFACE(*=)
    TEST_IFACE(/=)
}; // template <typename STI> struct TestAssignableInterfaces<STI>
#undef TEST_IFACE

template <typename STI,
          bool = std::is_same<bool, typename STI::ValueType>::value>
struct TestPreOpInterfaces: std::true_type {
    SA(std::is_same<decltype(++(std::declval<STI &>())),
                    STI &>::value);
    SA(noexcept(++(std::declval<STI &>())));
    SA(std::is_same<decltype(--(std::declval<STI &>())),
                    STI &>::value);
    SA(noexcept(--(std::declval<STI &>())));
}; // template <typename STI, bool> struct TestPreOpInterfaces
template <typename STI>
struct TestPreOpInterfaces<STI, true> : std::true_type{};

template <typename STI,
          bool = std::is_same<bool, typename STI::ValueType>::value>
struct TestPostOpInterfaces: std::true_type {
    SA(std::is_same<decltype(std::declval<STI &>()++),
                    STI>::value);
    SA(noexcept(std::declval<STI &>()++));
    SA(std::is_same<decltype(std::declval<STI &>()--),
                    STI>::value);
    SA(noexcept(std::declval<STI &>()--));
}; // template <typename STI, bool> struct TestPostOpInterfaces
template <typename STI>
struct TestPostOpInterfaces<STI, true> : std::true_type {};

template <typename STI>
struct TestStreamableInterface: std::true_type {
    SA(std::is_same<
            decltype(std::declval<std::ostream &>()
                     << std::declval<STI const &>()),
            std::ostream &>::value);
    SA(!noexcept(std::declval<std::ostream &>()
                 << std::declval<STI const &>()));
}; // template <typename STI> struct TestStreamableInterface

template <typename STI, typename Ostream>
struct TestStreamableToInterface: std::true_type {
    SA(std::is_same<
            decltype(std::declval<Ostream &>() << std::declval<STI const &>()),
            Ostream &>::value);
    SA(!noexcept(std::declval<Ostream &>() << std::declval<STI const &>()));
}; // template <typename STI, typename Ostream> struct TestStreamableToInterface

template <typename T>
struct CustomOutputStream {
    friend CustomOutputStream & operator<<(CustomOutputStream & os, T const &)
    { return os; }
}; // template <typename T> struct CustomOutputStream

template <typename STI>
struct TestSwappableInterface: std::true_type {
    SA(std::is_same<
            decltype(std::swap(std::declval<STI &>(), std::declval<STI &>())),
            void>::value);
    SA(noexcept(std::swap(std::declval<STI &>(), std::declval<STI &>())));
}; // template <typename STI> struct TestHashableInterface

template <typename T, typename Tag>
struct Test: std::true_type {
    using STI =
            sharemind::StrongType<
                T,
                Tag,
                sharemind::StrongTypeHashable,
                sharemind::StrongTypeEqualityComparable,
                sharemind::StrongTypeInequalityComparable,
                sharemind::StrongTypeLessThanComparable,
                sharemind::StrongTypeLessOrEqualComparable,
                sharemind::StrongTypeGreaterOrEqualComparable,
                sharemind::StrongTypeGreaterThanComparable,
                sharemind::StrongTypeAddAssignable,
                sharemind::StrongTypeSubAssignable,
                sharemind::StrongTypeMulAssignable,
                sharemind::StrongTypeDivAssignable,
                sharemind::StrongTypeModAssignable,
                sharemind::StrongTypeXorAssignable,
                sharemind::StrongTypeAndAssignable,
                sharemind::StrongTypeOrAssignable,
                sharemind::StrongTypeAddAssignableWith<T>,
                sharemind::StrongTypeSubAssignableWith<T>,
                sharemind::StrongTypeMulAssignableWith<T>,
                sharemind::StrongTypeDivAssignableWith<T>,
                sharemind::StrongTypeModAssignableWith<T>,
                sharemind::StrongTypeXorAssignableWith<T>,
                sharemind::StrongTypeAndAssignableWith<T>,
                sharemind::StrongTypeOrAssignableWith<T>,
                sharemind::StrongTypeIncrementable,
                sharemind::StrongTypeDecrementable,
                sharemind::StrongTypeSwappable,
                sharemind::StrongTypeStreamable,
                sharemind::StrongTypeStreamableTo<CustomOutputStream<T> >
            >;
    SA(TestCommonInterface<T, STI>::value);
    SA(TestHashableInterface<STI>::value);
    SA(TestComparableInterfaces<STI>::value);
    SA(TestAssignableInterfaces<STI>::value);
    SA(TestPreOpInterfaces<STI>::value);
    SA(TestPostOpInterfaces<STI>::value);
    SA(TestSwappableInterface<STI>::value);
    SA(TestStreamableInterface<STI>::value);
    SA(TestStreamableToInterface<STI, CustomOutputStream<T> >::value);

    SA(sharemind::IsStrongType<STI>::value);
    SA(!sharemind::IsStrongType<T>::value);
}; // template <typename T, typename Tag> struct Test_

#define TEST_(tag, ...) SA(Test<__VA_ARGS__, struct TypeTag_ ## tag>::value)
#define TEST(type) TEST_(type, type)
#define TEST_STD(type) TEST_(type, std::type)
#define TEST_STD_W(type) TEST_STD(type ## 8_t); TEST_STD(type ## 16_t); \
                         TEST_STD(type ## 32_t); TEST_STD(type ## 64_t)

TEST(float);
TEST(double);
TEST_(long_double, long double);
TEST(bool);
TEST(char);
TEST_(schar, signed char);
TEST_(uchar, unsigned char);
TEST(char16_t);
TEST(char32_t);
TEST(wchar_t);
TEST_(shortInt, short int);
TEST(int);
TEST_(longInt, long int);
TEST_(longLongInt, long long int);
TEST_(ushortInt, unsigned short int);
TEST_(unsignedInt, unsigned int);
TEST_(unsignedLongInt, unsigned long int);
TEST_(unsignedLongLongInt, unsigned long long int);
TEST_STD_W(int); TEST_STD_W(int_least); TEST_STD_W(int_fast);
TEST_STD_W(uint); TEST_STD_W(uint_least); TEST_STD_W(uint_fast);
TEST_STD(intptr_t); TEST_STD(uintptr_t);
TEST_STD(intmax_t); TEST_STD(uintmax_t);

int main() {}
