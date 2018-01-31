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

#ifndef SHAREMIND_OPTIONAL_H
#define SHAREMIND_OPTIONAL_H

#include <cassert>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>
#include "compiler-support/ClangPR22637.h"
#include "Concepts.h"


namespace sharemind {

struct NullOption {

private: /* Types: */

    struct Secret {};

public: /* Methods: */

    NullOption() = delete;
    template <typename T> NullOption(std::initializer_list<T> const &) = delete;

    constexpr static NullOption getInstance() noexcept
    { return NullOption(Secret(), Secret()); }

private: /* Methods: */

    constexpr explicit NullOption(Secret, Secret) noexcept {}

};
constexpr NullOption nullOption = NullOption::getInstance();
struct InPlace { explicit InPlace() = default; };
constexpr InPlace inPlace = InPlace();

namespace Detail {
namespace Optional {

template <typename T, bool = std::is_trivially_destructible<T>::value>
class Impl;

/* Constexpr implied const in C++11, but not in C++14. */
#if __cplusplus >= 201402L
#define SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR constexpr
#else
#define SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR
#endif

#define SHAREMIND_OPTIONAL_H_IMPL_COMMON \
    static_assert(std::is_nothrow_destructible<T>::value, ""); \
    static_assert(!std::is_reference<T>::value, ""); \
    static_assert(!std::is_same<typename std::remove_cv<T>::type, InPlace>::value, ""); \
    static_assert(!std::is_same<typename std::remove_cv<T>::type, NullOption>::value, ""); \
public: /* Methods: */ \
    constexpr Impl() noexcept : m_empty(), m_valid(false) {} \
    constexpr Impl(NullOption) noexcept : m_empty(), m_valid(false) {} \
    template <typename ... Args> \
    constexpr Impl(InPlace, Args && ... args) \
            noexcept(std::is_nothrow_constructible<T, Args...>::value) \
        : m_data(std::forward<Args>(args)...) \
        , m_valid(true) \
    {} \
    Impl(Impl && move) noexcept(std::is_nothrow_move_constructible<T>::value) \
        : m_valid(move.m_valid) \
    { \
        if (m_valid) \
            new (std::addressof(m_data)) T(std::move(move.m_data)); \
    } \
    Impl(Impl & copy) noexcept(std::is_nothrow_copy_constructible<T>::value) \
        : Impl(const_cast<Impl const &>(copy)) \
    {} \
    Impl(Impl const & copy) \
            noexcept(std::is_nothrow_copy_constructible<T>::value) \
        : m_valid(copy.m_valid) \
    { \
        if (m_valid) \
            new (std::addressof(m_data)) T(copy.m_data); \
    } \
    Impl & operator=(Impl && move) \
            noexcept(std::is_nothrow_move_assignable<T>::value \
                     && std::is_nothrow_move_constructible<T>::value) \
    { \
        if (&move != this) { \
            if (m_valid) { \
                if (move.m_valid) { \
                    m_data = std::move(move.m_data); \
                } else { \
                    m_data.~T(); \
                    m_valid = false; \
                } \
            } else { \
                if (move.m_valid) { \
                    new (std::addressof(m_data)) T(std::move(move.m_data)); \
                    m_valid = true; \
                } \
            } \
        } \
        return *this; \
    } \
    Impl & operator=(Impl const & copy) \
            noexcept(std::is_nothrow_copy_assignable<T>::value \
                     && std::is_nothrow_copy_constructible<T>::value) \
    { \
        if (&copy != this) { \
            if (m_valid) { \
                if (copy.m_valid) { \
                    m_data = copy.m_data; \
                } else { \
                    m_data.~T(); \
                    m_valid = false; \
                } \
            } else { \
                if (copy.m_valid) { \
                    new (std::addressof(m_data)) T(copy.m_data); \
                    m_valid = true; \
                } \
            } \
        } \
        return *this; \
    } \
    constexpr explicit operator bool() const noexcept { return m_valid; } \
    constexpr T const * operator->() const noexcept \
    { return (assert(m_valid), std::addressof(m_data)); } \
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T * operator->() noexcept \
    { return (assert(m_valid), std::addressof(m_data)); } \
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T & operator*() & noexcept \
    { return (assert(m_valid), m_data); } \
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T && operator*() && noexcept \
    { return (assert(m_valid), std::move(m_data)); } \
    constexpr T const & operator*() const & noexcept \
    { return (assert(m_valid), m_data); } \
    constexpr T const && operator*() const && noexcept \
    { return (assert(m_valid), std::move(m_data)); } \
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T & value() & noexcept \
    { return (assert(m_valid), m_data); } \
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T && value() && noexcept \
    { return (assert(m_valid), std::move(m_data)); } \
    constexpr T const & value() const & noexcept \
    { return (assert(m_valid), m_data); } \
    constexpr T const && value() const && noexcept \
    { return (assert(m_valid), std::move(m_data)); } \
    template <typename ... Args> \
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T value(Args && ... args) && \
            noexcept \
    { return m_valid ? std::move(m_data) : T(std::forward<Args>(args)...); } \
    template <typename ... Args> \
    constexpr T value(Args && ... args) const & noexcept { \
        return m_valid \
               ? T(SHAREMIND_CLANGPR22637_WORKAROUND(m_data)) \
               : T(std::forward<Args>(args)...); \
    } \
private: /* Fields: */ \
    union { \
        char m_empty; \
        T m_data; \
    }; \
    bool m_valid;

template <typename T>
class Impl<T, false> {

    SHAREMIND_OPTIONAL_H_IMPL_COMMON

public: /* Methods: */

    ~Impl() noexcept {
        if (m_valid)
            m_data.~T();
    }

    void reset() noexcept {
        if (m_valid) {
            m_data.~T();
            m_valid = false;
        }
    }

    template <typename ... Args>
    void emplace(Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
    {
        if (m_valid) {
            m_data.~T();
            m_valid = false;
        }
        new (std::addressof(m_data)) T(std::forward<Args>(args)...);
        m_valid = true;
    }

};

template <typename T>
class Impl<T, true> {

    SHAREMIND_OPTIONAL_H_IMPL_COMMON

public: /* Methods: */

    void reset() noexcept { m_valid = false; }

    template <typename ... Args>
    void emplace(Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
    {
        m_valid = false;
        new (std::addressof(m_data)) T(std::forward<Args>(args)...);
        m_valid = true;
    }

};

#undef SHAREMIND_OPTIONAL_H_IMPL_COMMON
#undef SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR

} /* namespace Optional { */
} /* namespace Detail { */

template <typename T>
using Optional = Detail::Optional::Impl<T>;

template <typename T, typename U>
constexpr auto operator==(Optional<T> const & lhs, Optional<U> const & rhs)
        noexcept(noexcept(*lhs == *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, EqualityComparable(T, U))
{
    return (bool(lhs) != bool(rhs))
           ? false
           : (bool(lhs) == false ? true : (*lhs == *rhs));
}

template <typename T, typename U>
constexpr auto operator!=(Optional<T> const & lhs, Optional<U> const & rhs)
        noexcept(noexcept(*lhs == *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InequalityComparable(T, U))
{
    return (bool(lhs) != bool(rhs))
           ? true
           : (bool(lhs) == false ? false : (*lhs != *rhs));
}

template <typename T, typename U>
constexpr auto operator<(Optional<T> const & lhs, Optional<U> const & rhs)
        noexcept(noexcept(*lhs < *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, LessThanComparable(T, U))
{ return !rhs ? false : (!lhs ? true : (*lhs < *rhs)); }

template <typename T, typename U>
constexpr auto operator<=(Optional<T> const & lhs, Optional<U> const & rhs)
        noexcept(noexcept(*lhs <= *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, LessOrEqualComparable(T, U))
{ return !lhs ? true : (!rhs ? false : (*lhs <= *rhs)); }

template <typename T, typename U>
constexpr auto operator>(Optional<T> const & lhs, Optional<U> const & rhs)
        noexcept(noexcept(*lhs > *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, GreaterThanComparable(T, U))
{ return !lhs ? false : (!rhs ? true : (*lhs > *rhs)); }

template <typename T, typename U>
constexpr auto operator>=(Optional<T> const & lhs, Optional<U> const & rhs)
        noexcept(noexcept(*lhs >= *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, GreaterOrEqualComparable(T, U))
{ return !rhs ? true : (!lhs ? false : (*lhs >= *rhs)); }

#define SHAREMIND_OPTIONAL_H_NULLCOMPARE(op,r1,r2) \
    template <typename T> \
    constexpr bool operator op(Optional<T> const & x, NullOption) noexcept \
    { return r1; } \
    template <typename T> \
    constexpr bool operator op(NullOption, Optional<T> const & x) noexcept \
    { return r2; }
SHAREMIND_OPTIONAL_H_NULLCOMPARE(==, !x, !x)
SHAREMIND_OPTIONAL_H_NULLCOMPARE(!=, bool(x), bool(x))
SHAREMIND_OPTIONAL_H_NULLCOMPARE(<,  (static_cast<void>(x), false), bool(x))
SHAREMIND_OPTIONAL_H_NULLCOMPARE(<=, !x, (static_cast<void>(x), true))
SHAREMIND_OPTIONAL_H_NULLCOMPARE(>,  bool(x), (static_cast<void>(x), false))
SHAREMIND_OPTIONAL_H_NULLCOMPARE(>=, (static_cast<void>(x), true), !x)
#undef SHAREMIND_OPTIONAL_H_NULLCOMPARE

#define SHAREMIND_OPTIONAL_H_VALUECOMPARE(op,r1,r2) \
    template <typename T, typename U> \
    constexpr bool operator op(Optional<T> const & x, U const & v) noexcept \
    { return bool(x) ? (*x op v) : r1; } \
    template <typename T, typename U> \
    constexpr bool operator op(T const & v, Optional<U> const & x) noexcept \
    { return bool(x) ? (v op *x) : r2; }
SHAREMIND_OPTIONAL_H_VALUECOMPARE(==, false, false)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(!=, true,  true)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(<,  true,  false)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(<=, true,  false)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(>,  false, true)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(>=, false, true)
#undef SHAREMIND_OPTIONAL_H_VALUECOMPARE

} /* namespace sharemind */

#endif /* SHAREMIND_OPTIONAL_H */
