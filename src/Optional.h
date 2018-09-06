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
#include "RemoveCvref.h"


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
struct DestructorBase;

template <typename T>
struct DestructorBase<T, true> {

/* Methods: */

    constexpr DestructorBase() noexcept
        : m_noValuePlaceholder()
        , m_containsValue(false)
    {}

    constexpr explicit DestructorBase(bool const valid) noexcept
        : m_noValuePlaceholder()
        , m_containsValue(valid)
    {}

    template <typename ... Args>
    constexpr explicit DestructorBase(InPlace, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args &&...>::value)
        : m_data(std::forward<Args>(args)...)
        , m_containsValue(true)
    {}

    void reset() noexcept { this->m_containsValue = false; }

/* Fields: */

    union {
        char m_noValuePlaceholder;
        T m_data;
    };
    bool m_containsValue;

};

template <typename T>
struct DestructorBase<T, false> {

/* Methods: */

    constexpr DestructorBase() noexcept
        : m_noValuePlaceholder()
        , m_containsValue(false)
    {}

    constexpr explicit DestructorBase(bool const valid) noexcept
        : m_noValuePlaceholder()
        , m_containsValue(valid)
    {}

    template <typename ... Args>
    constexpr explicit DestructorBase(InPlace, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args &&...>::value)
        : m_data(std::forward<Args>(args)...)
        , m_containsValue(true)
    {}

    ~DestructorBase() noexcept {
        if (m_containsValue)
            m_data.~T();
    }

    void reset() noexcept {
        if (m_containsValue) {
            m_data.~T();
            static_assert(std::is_nothrow_destructible<T>::value,
                          "T is required to be model Destructible!");
            m_containsValue = false;
        }
    }

/* Fields: */

    union {
        char m_noValuePlaceholder;
        T m_data;
    };
    bool m_containsValue;

};

#define SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(name,base) \
    constexpr name() noexcept = default; \
    constexpr explicit name(bool const v) noexcept : base<T>(v) {} \
    template <typename ... Args> \
    constexpr explicit name(InPlace ip, Args && ... args) \
            noexcept(std::is_nothrow_constructible<T, Args &&...>::value) \
        : base<T>(std::move(ip), std::forward<Args>(args)...) \
    {}

template <typename T,
          int = std::is_copy_constructible<T>::value
                ? (std::is_trivially_copy_constructible<T>::value ? 1 : 2)
                : 0>
struct CopyConstructorBase: DestructorBase<T>
{ using DestructorBase<T>::DestructorBase; };

template <typename T>
struct CopyConstructorBase<T, 0>: DestructorBase<T> {
    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(CopyConstructorBase, DestructorBase)
    CopyConstructorBase(CopyConstructorBase const &)             = delete;
    CopyConstructorBase(CopyConstructorBase &&)                  = default;
    CopyConstructorBase & operator=(CopyConstructorBase const &) = default;
    CopyConstructorBase & operator=(CopyConstructorBase &&)      = default;

};

template <typename T>
struct CopyConstructorBase<T, 2>: DestructorBase<T> {

    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(CopyConstructorBase, DestructorBase)

    CopyConstructorBase(CopyConstructorBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value)
        : DestructorBase<T>(copy.m_containsValue)
    {
        if (copy.m_containsValue)
            new (std::addressof(this->m_data)) T(copy.m_data);
    }

    constexpr CopyConstructorBase(CopyConstructorBase &&) = default;

    CopyConstructorBase & operator=(CopyConstructorBase const &) = default;

    CopyConstructorBase & operator=(CopyConstructorBase &&) = default;

};

template <typename T, bool = std::is_trivially_move_constructible<T>::value>
struct MoveConstructorBase: CopyConstructorBase<T>
{ using CopyConstructorBase<T>::CopyConstructorBase; };

template <typename T>
struct MoveConstructorBase<T, false>: CopyConstructorBase<T> {

    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(MoveConstructorBase,
                                           CopyConstructorBase)

    MoveConstructorBase(MoveConstructorBase const & copy) = default;

    MoveConstructorBase(MoveConstructorBase && move)
            noexcept(std::is_nothrow_move_constructible<T>::value)
        : CopyConstructorBase<T>(move.m_containsValue)
    {
        if (move.m_containsValue)
            new (std::addressof(this->m_data)) T(std::move(move.m_data));
    }

    MoveConstructorBase & operator=(MoveConstructorBase const &) = default;

    MoveConstructorBase & operator=(MoveConstructorBase &&) = default;

};

template <typename T,
          bool = std::is_copy_constructible<T>::value
                 && std::is_copy_assignable<T>::value>
struct CopyAssignmentBase: MoveConstructorBase<T> {
    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(CopyAssignmentBase,
                                           MoveConstructorBase)
    CopyAssignmentBase(CopyAssignmentBase const &)             = default;
    CopyAssignmentBase(CopyAssignmentBase &&)                  = default;
    CopyAssignmentBase & operator=(CopyAssignmentBase const &) = delete;
    CopyAssignmentBase & operator=(CopyAssignmentBase &&)      = default;

};

template <typename T>
struct CopyAssignmentBase<T, true>: MoveConstructorBase<T> {

    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(CopyAssignmentBase,
                                           MoveConstructorBase)

    CopyAssignmentBase(CopyAssignmentBase const & copy) = default;

    CopyAssignmentBase(CopyAssignmentBase && move) = default;

    CopyAssignmentBase & operator=(CopyAssignmentBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value
                     && std::is_nothrow_copy_assignable<T>::value)
    {
        if (copy.m_containsValue) {
            if (this->m_containsValue) {
                this->m_data = copy.m_data;
            } else {
                new (std::addressof(this->m_data)) T(copy.m_data);
                this->m_containsValue = true;
            }
        } else {
            this->reset();
        }
        return *this;
    }

    CopyAssignmentBase & operator=(CopyAssignmentBase &&) = default;

};

template <typename T,
          bool = std::is_move_constructible<T>::value
                 && std::is_move_assignable<T>::value>
struct MoveAssignmentBase: CopyAssignmentBase<T> {
    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(MoveAssignmentBase,
                                           CopyAssignmentBase)
    MoveAssignmentBase(MoveAssignmentBase const &)             = default;
    MoveAssignmentBase(MoveAssignmentBase &&)                  = default;
    MoveAssignmentBase & operator=(MoveAssignmentBase const &) = default;
    MoveAssignmentBase & operator=(MoveAssignmentBase &&)      = delete;
};

template <typename T>
struct MoveAssignmentBase<T, true>: CopyAssignmentBase<T> {

    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(MoveAssignmentBase,
                                           CopyAssignmentBase)

    MoveAssignmentBase(MoveAssignmentBase const & copy) = default;

    MoveAssignmentBase(MoveAssignmentBase && move) = default;

    MoveAssignmentBase & operator=(MoveAssignmentBase const & copy) = default;

    MoveAssignmentBase & operator=(MoveAssignmentBase && move)
            noexcept(std::is_nothrow_move_constructible<T>::value
                     && std::is_nothrow_move_assignable<T>::value)
    {
        if (move.m_containsValue) {
            if (this->m_containsValue) {
                this->m_data = std::move(move.m_data);
            } else {
                new (std::addressof(this->m_data)) T(std::move(move.m_data));
                this->m_containsValue = true;
            }
        } else {
            this->reset();
        }
        return *this;
    }

};


/* Constexpr implied const in C++11, but not in C++14. */
#if __cplusplus >= 201402L
#define SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR constexpr
#else
#define SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR
#endif

template <typename T>
class Impl : private MoveAssignmentBase<T> {

    static_assert(std::is_object<T>::value, "");

private: /* Types: */

    template <typename U, bool isExplicit>
    using IsPassThroughConstructorArg =
            std::integral_constant<
                bool,
                !std::is_same<RemoveCvrefT<U>, InPlace>::value
                && !std::is_same<RemoveCvrefT<U>, Impl>::value
                && std::is_constructible<T, U &&>::value
                && (std::is_convertible<U &&, T>::value != isExplicit)
            >;

public: /* Methods: */

    SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH(Impl, MoveAssignmentBase)
    #undef SHAREMIND_OPTIONAL_H_PROXY_PASSTHROUGH

    constexpr Impl(NullOption) noexcept : Impl() {}

    constexpr Impl(Impl const &)
            noexcept(std::is_nothrow_copy_constructible<T>::value) = default;

    constexpr Impl(Impl &&)
            noexcept(std::is_nothrow_move_constructible<T>::value) = default;

    template <typename U = T,
              typename std::enable_if<
                    IsPassThroughConstructorArg<U, false>::value,
                    int>::type = 0>
    constexpr Impl(U && v) : Impl(inPlace, std::forward<U>(v)) {}

    template <typename U = T,
              typename std::enable_if<
                    IsPassThroughConstructorArg<U, true>::value,
                    int>::type = 0>
    explicit constexpr Impl(U && v) : Impl(inPlace, std::forward<U>(v)) {}

    Impl & operator=(NullOption) noexcept {
        this->reset();
        return *this;
    }

    Impl & operator=(Impl const &)
            noexcept(std::is_nothrow_copy_constructible<T>::value
                     && std::is_nothrow_copy_assignable<T>::value) = default;

    Impl & operator=(Impl &&)
            noexcept(std::is_nothrow_move_constructible<T>::value
                     && std::is_nothrow_move_assignable<T>::value) = default;

    constexpr T const * operator->() const noexcept
    { return (assert(this->m_containsValue), std::addressof(this->m_data)); }

    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T * operator->() noexcept
    { return (assert(this->m_containsValue), std::addressof(this->m_data)); }

    constexpr T const & operator*() const & noexcept
    { return (assert(this->m_containsValue), this->m_data); }

    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T & operator*() & noexcept
    { return (assert(this->m_containsValue), this->m_data); }

    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T && operator*() && noexcept
    { return (assert(this->m_containsValue), std::move(this->m_data)); }

    constexpr T const && operator*() const && noexcept
    { return (assert(this->m_containsValue), std::move(this->m_data)); }

    constexpr explicit operator bool() const noexcept
    { return this->m_containsValue; }

    constexpr bool hasValue() const noexcept { return this->m_containsValue; }

    constexpr T const & value() const & noexcept
    { return (assert(this->m_containsValue), this->m_data); }

    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T & value() & noexcept
    { return (assert(this->m_containsValue), this->m_data); }

    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T && value() && noexcept
    { return (assert(this->m_containsValue), std::move(this->m_data)); }

    constexpr T const && value() const && noexcept
    { return (assert(this->m_containsValue), std::move(this->m_data)); }

    template <typename ... Args>
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T value(Args && ... args) &&
            noexcept
    {
        return this->m_containsValue
               ? std::move(this->m_data)
               : T(std::forward<Args>(args)...);
    }

    template <typename ... Args>
    constexpr T value(Args && ... args) const & noexcept {
        return this->m_containsValue
               ? T(SHAREMIND_CLANGPR22637_WORKAROUND(this->m_data))
               : T(std::forward<Args>(args)...);
    }

    template <typename ... Args>
    T & emplace(Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args &&...>::value)
    {
        this->reset();
        new (std::addressof(this->m_data)) T(std::forward<Args>(args)...);
        this->m_containsValue = true;
        return this->m_data;
    }

    template <typename U, typename ... Args>
    auto emplace(std::initializer_list<U> il, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args &&...>::value)
            -> typename std::enable_if<
                    std::is_constructible<
                        T,
                        std::initializer_list<U> &,
                        Args &&...
                    >::value,
                    T &>::type
    {
        this->reset();
        new (std::addressof(this->m_data)) T(il, std::forward<Args>(args)...);
        this->m_containsValue = true;
        return this->m_data;
    }
};

#undef SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR

template <typename T, typename U>
constexpr auto operator==(Impl<T> const & lhs, Impl<U> const & rhs)
        noexcept(noexcept(*lhs == *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, EqualityComparable(T, U))
{
    return (bool(lhs) != bool(rhs))
           ? false
           : (bool(lhs) == false ? true : (*lhs == *rhs));
}

template <typename T, typename U>
constexpr auto operator!=(Impl<T> const & lhs, Impl<U> const & rhs)
        noexcept(noexcept(*lhs == *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, InequalityComparable(T, U))
{
    return (bool(lhs) != bool(rhs))
           ? true
           : (bool(lhs) == false ? false : (*lhs != *rhs));
}

template <typename T, typename U>
constexpr auto operator<(Impl<T> const & lhs, Impl<U> const & rhs)
        noexcept(noexcept(*lhs < *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, LessThanComparable(T, U))
{ return !rhs ? false : (!lhs ? true : (*lhs < *rhs)); }

template <typename T, typename U>
constexpr auto operator<=(Impl<T> const & lhs, Impl<U> const & rhs)
        noexcept(noexcept(*lhs <= *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, LessOrEqualComparable(T, U))
{ return !lhs ? true : (!rhs ? false : (*lhs <= *rhs)); }

template <typename T, typename U>
constexpr auto operator>(Impl<T> const & lhs, Impl<U> const & rhs)
        noexcept(noexcept(*lhs > *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, GreaterThanComparable(T, U))
{ return !lhs ? false : (!rhs ? true : (*lhs > *rhs)); }

template <typename T, typename U>
constexpr auto operator>=(Impl<T> const & lhs, Impl<U> const & rhs)
        noexcept(noexcept(*lhs >= *rhs))
        -> SHAREMIND_REQUIRE_CONCEPTS_R(bool, GreaterOrEqualComparable(T, U))
{ return !rhs ? true : (!lhs ? false : (*lhs >= *rhs)); }

#define SHAREMIND_OPTIONAL_H_NULLCOMPARE(op,r1,r2) \
    template <typename T> \
    constexpr bool operator op(Impl<T> const & x, NullOption) noexcept \
    { return r1; } \
    template <typename T> \
    constexpr bool operator op(NullOption, Impl<T> const & x) noexcept \
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
    constexpr bool operator op(Impl<T> const & x, U const & v) noexcept \
    { return bool(x) ? (*x op v) : r1; } \
    template <typename T, typename U> \
    constexpr bool operator op(T const & v, Impl<U> const & x) noexcept \
    { return bool(x) ? (v op *x) : r2; }
SHAREMIND_OPTIONAL_H_VALUECOMPARE(==, false, false)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(!=, true,  true)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(<,  true,  false)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(<=, true,  false)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(>,  false, true)
SHAREMIND_OPTIONAL_H_VALUECOMPARE(>=, false, true)
#undef SHAREMIND_OPTIONAL_H_VALUECOMPARE

} /* namespace Optional { */
} /* namespace Detail { */

template <typename T>
using Optional = Detail::Optional::Impl<T>;

template <typename T>
constexpr Optional<typename std::decay<T>::type> makeOptional(T && v)
{ return Optional<typename std::decay<T>::type>(std::forward<T>(v)); }

template <typename T, typename ... Args>
constexpr Optional<T> makeOptional(Args && ... args)
{ return Optional<T>(inPlace, std::forward<Args>(args)...); }

template <typename T, typename U, typename ... Args>
constexpr Optional<T> makeOptional(std::initializer_list<U> l, Args && ... args)
{ return Optional<T>(inPlace, l, std::forward<Args>(args)...); }

} /* namespace sharemind */

#endif /* SHAREMIND_OPTIONAL_H */
