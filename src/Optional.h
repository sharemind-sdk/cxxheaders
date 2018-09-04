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

    constexpr DestructorBase() noexcept : m_empty(), m_valid(false) {}

    constexpr DestructorBase(bool const valid) noexcept
        : m_empty()
        , m_valid(valid)
    {}

    template <typename ... Args>
    constexpr DestructorBase(InPlace, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : m_data(std::forward<Args>(args)...)
        , m_valid(true)
    {}

    void reset() noexcept { this->m_valid = false; }

/* Fields: */

    union {
        char m_empty;
        T m_data;
    };
    bool m_valid;

};

template <typename T>
struct DestructorBase<T, false> {

/* Methods: */

    constexpr DestructorBase() noexcept : m_empty(), m_valid(false) {}

    constexpr DestructorBase(bool const valid) noexcept
        : m_empty()
        , m_valid(valid)
    {}

    template <typename ... Args>
    constexpr DestructorBase(InPlace, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : m_data(std::forward<Args>(args)...)
        , m_valid(true)
    {}

    ~DestructorBase() noexcept {
        if (m_valid)
            m_data.~T();
    }

    void reset() noexcept {
        if (m_valid) {
            m_data.~T();
            m_valid = false;
        }
    }

/* Fields: */

    union {
        char m_empty;
        T m_data;
    };
    bool m_valid;

};

template <typename T, bool = std::is_trivially_copy_constructible<T>::value>
struct CopyConstructorBase: DestructorBase<T>
{ using DestructorBase<T>::DestructorBase; };

template <typename T>
struct CopyConstructorBase<T, false>: DestructorBase<T> {

    constexpr CopyConstructorBase() noexcept = default;

    constexpr CopyConstructorBase(bool const v) noexcept
        : DestructorBase<T>(v)
    {}

    template <typename ... Args>
    constexpr CopyConstructorBase(InPlace ip, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : DestructorBase<T>(std::move(ip), std::forward<Args>(args)...)
    {}

    CopyConstructorBase(CopyConstructorBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value)
        : DestructorBase<T>(copy.m_valid)
    {
        if (copy.m_valid)
            new (std::addressof(this->m_data)) T(copy.m_data);
    }

    constexpr CopyConstructorBase(CopyConstructorBase &&)
            noexcept(std::is_nothrow_move_constructible<T>::value) = default;

    CopyConstructorBase & operator=(CopyConstructorBase const &)
            noexcept(std::is_nothrow_copy_constructible<T>::value
                     && std::is_nothrow_copy_assignable<T>::value) = default;

    CopyConstructorBase & operator=(CopyConstructorBase &&)
            noexcept(std::is_nothrow_move_constructible<T>::value
                     && std::is_nothrow_move_assignable<T>::value) = default;

};

template <typename T, bool = std::is_trivially_move_constructible<T>::value>
struct MoveConstructorBase: CopyConstructorBase<T>
{ using CopyConstructorBase<T>::CopyConstructorBase; };

template <typename T>
struct MoveConstructorBase<T, false>: CopyConstructorBase<T> {

    constexpr MoveConstructorBase() noexcept = default;

    constexpr MoveConstructorBase(bool const v) noexcept
        : CopyConstructorBase<T>(v)
    {}

    template <typename ... Args>
    constexpr MoveConstructorBase(InPlace ip, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : CopyConstructorBase<T>(std::move(ip), std::forward<Args>(args)...)
    {}

    MoveConstructorBase(MoveConstructorBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value) = default;

    MoveConstructorBase(MoveConstructorBase && move)
            noexcept(std::is_nothrow_move_constructible<T>::value)
        : CopyConstructorBase<T>(move.m_valid)
    {
        if (move.m_valid)
            new (std::addressof(this->m_data)) T(std::move(move.m_data));
    }

    MoveConstructorBase & operator=(MoveConstructorBase const &)
            noexcept(std::is_nothrow_copy_constructible<T>::value
                     && std::is_nothrow_copy_assignable<T>::value) = default;

    MoveConstructorBase & operator=(MoveConstructorBase &&)
            noexcept(std::is_nothrow_move_constructible<T>::value
                     && std::is_nothrow_move_assignable<T>::value) = default;

};

template <typename T, bool = std::is_trivially_copy_assignable<T>::value>
struct CopyAssignmentBase: MoveConstructorBase<T>
{ using MoveConstructorBase<T>::MoveConstructorBase; };

template <typename T>
struct CopyAssignmentBase<T, false>: MoveConstructorBase<T> {

    constexpr CopyAssignmentBase() noexcept = default;

    constexpr CopyAssignmentBase(bool const v) noexcept
        : MoveConstructorBase<T>(v)
    {}

    template <typename ... Args>
    constexpr CopyAssignmentBase(InPlace ip, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : MoveConstructorBase<T>(std::move(ip), std::forward<Args>(args)...)
    {}

    CopyAssignmentBase(CopyAssignmentBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value) = default;

    CopyAssignmentBase(CopyAssignmentBase && move)
            noexcept(std::is_nothrow_move_constructible<T>::value) = default;

    CopyAssignmentBase & operator=(CopyAssignmentBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value
                     && std::is_nothrow_copy_assignable<T>::value)
    {
        if (copy.m_valid) {
            if (this->m_valid) {
                this->m_data = copy.m_data;
            } else {
                new (std::addressof(this->m_data)) T(copy.m_data);
                this->m_valid = true;
            }
        } else {
            this->reset();
        }
        return *this;
    }

    CopyAssignmentBase & operator=(CopyAssignmentBase &&)
            noexcept(std::is_nothrow_move_constructible<T>::value
                     && std::is_nothrow_move_assignable<T>::value) = default;

};

template <typename T, bool = std::is_trivially_copy_assignable<T>::value>
struct MoveAssignmentBase: CopyAssignmentBase<T>
{ using CopyAssignmentBase<T>::CopyAssignmentBase; };

template <typename T>
struct MoveAssignmentBase<T, false>: CopyAssignmentBase<T> {

    constexpr MoveAssignmentBase() noexcept = default;

    constexpr MoveAssignmentBase(bool const v) noexcept
        : CopyAssignmentBase<T>(v)
    {}

    template <typename ... Args>
    constexpr MoveAssignmentBase(InPlace ip, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : CopyAssignmentBase<T>(std::move(ip), std::forward<Args>(args)...)
    {}

    MoveAssignmentBase(MoveAssignmentBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value) = default;

    MoveAssignmentBase(MoveAssignmentBase && move)
            noexcept(std::is_nothrow_move_constructible<T>::value) = default;

    MoveAssignmentBase & operator=(MoveAssignmentBase const & copy)
            noexcept(std::is_nothrow_copy_constructible<T>::value
                     && std::is_nothrow_copy_assignable<T>::value) = default;

    MoveAssignmentBase & operator=(MoveAssignmentBase && move)
            noexcept(std::is_nothrow_move_constructible<T>::value
                     && std::is_nothrow_move_assignable<T>::value)
    {
        if (move.m_valid) {
            if (this->m_valid) {
                this->m_data = std::move(move.m_data);
            } else {
                new (std::addressof(this->m_data)) T(std::move(move.m_data));
                this->m_valid = true;
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

    constexpr Impl() noexcept = default;

    constexpr Impl(NullOption) noexcept : Impl() {}

    constexpr Impl(Impl const &)
            noexcept(std::is_nothrow_copy_constructible<T>::value) = default;

    constexpr Impl(Impl &&)
            noexcept(std::is_nothrow_move_constructible<T>::value) = default;

    template <typename ... Args>
    constexpr Impl(InPlace ip, Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
        : MoveAssignmentBase<T>(std::move(ip), std::forward<Args>(args)...)
    {}

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

    Impl & operator=(Impl const &)
            noexcept(std::is_nothrow_copy_constructible<T>::value
                     && std::is_nothrow_copy_assignable<T>::value) = default;

    Impl & operator=(Impl &&)
            noexcept(std::is_nothrow_move_constructible<T>::value
                     && std::is_nothrow_move_assignable<T>::value) = default;

    constexpr explicit operator bool() const noexcept { return this->m_valid; }
    constexpr T const * operator->() const noexcept
    { return (assert(this->m_valid), std::addressof(this->m_data)); }
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T * operator->() noexcept
    { return (assert(this->m_valid), std::addressof(this->m_data)); }
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T & operator*() & noexcept
    { return (assert(this->m_valid), this->m_data); }
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T && operator*() && noexcept
    { return (assert(this->m_valid), std::move(this->m_data)); }
    constexpr T const & operator*() const & noexcept
    { return (assert(this->m_valid), this->m_data); }
    constexpr T const && operator*() const && noexcept
    { return (assert(this->m_valid), std::move(this->m_data)); }
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T & value() & noexcept
    { return (assert(this->m_valid), this->m_data); }
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T && value() && noexcept
    { return (assert(this->m_valid), std::move(this->m_data)); }
    constexpr T const & value() const & noexcept
    { return (assert(this->m_valid), this->m_data); }
    constexpr T const && value() const && noexcept
    { return (assert(this->m_valid), std::move(this->m_data)); }
    template <typename ... Args>
    SHAREMIND_OPTIONAL_H_CXX14_CONSTEXPR T value(Args && ... args) &&
            noexcept
    { return this->m_valid ? std::move(this->m_data) : T(std::forward<Args>(args)...); }
    template <typename ... Args>
    constexpr T value(Args && ... args) const & noexcept {
        return this->m_valid
               ? T(SHAREMIND_CLANGPR22637_WORKAROUND(this->m_data))
               : T(std::forward<Args>(args)...);
    }

    template <typename ... Args>
    void emplace(Args && ... args)
            noexcept(std::is_nothrow_constructible<T, Args...>::value)
    {
        this->reset();
        new (std::addressof(this->m_data)) T(std::forward<Args>(args)...);
        this->m_valid = true;
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
