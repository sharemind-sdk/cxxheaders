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
                sharemind::StrongTypeSwappable,
                sharemind::StrongTypeStreamable,
                sharemind::StrongTypeStreamableTo<CustomOutputStream<T> >
            >;
    SA(TestCommonInterface<T, STI>::value);
    SA(TestHashableInterface<STI>::value);
    SA(TestComparableInterfaces<STI>::value);
    SA(TestAssignableInterfaces<STI>::value);
    SA(TestSwappableInterface<STI>::value);
    SA(TestStreamableInterface<STI>::value);
    SA(TestStreamableToInterface<STI, CustomOutputStream<T> >::value);
}; // template <typename T, typename Tag> struct Test_

#define TEST_(type, tag) SA(Test<type, struct TypeTag_ ## tag>::value)
#define TEST(type) TEST_(type, type)

TEST(float);
TEST(double);
TEST_(long double, long_double);
TEST(int);

int main() {}
