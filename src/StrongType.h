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

#ifndef SHAREMIND_STRONGTYPE_H
#define SHAREMIND_STRONGTYPE_H

#include <functional>
#include <iosfwd>
#include <type_traits>
#include <utility>
#include "IsSwappable.h"
#include "RemoveCvref.h"
#include "TemplateContainsType.h"


namespace sharemind {

struct StrongTypeHashable {
    template <typename T>
    struct impl {
        auto hash() const noexcept {
            return std::hash<typename T::ValueType>()(
                        static_cast<T const &>(*this).get());
        }
    };
};

namespace Detail {

template <typename ... Ts>
struct StrongTypeHashableCheck : std::integral_constant<bool, true> {};

template <typename T, typename ... Ts>
struct StrongTypeHashableCheck<T, Ts...>
        : std::integral_constant<
                bool,
                !TemplateContainsType<StrongTypeHashable, Ts...>::value
            >
{};

} /* namespace Detail */

template <typename T, typename Tag, typename ... Mixins>
class StrongType
        : public Mixins::template impl<StrongType<T, Tag, Mixins...> > ...
{

    static_assert(std::is_arithmetic<T>::value
                  || std::is_pointer<T>::value
                  || std::is_member_pointer<T>::value, "");
    static_assert(Detail::StrongTypeHashableCheck<Mixins...>::value,
                  "StrongTypeHashable needs to be the first mixin to work!");

public: /* Types: */

    using ValueType = T;

public: /* Methods: */

    constexpr StrongType() noexcept {}

    constexpr explicit StrongType(ValueType const & value) noexcept
        : m_value(value)
    {}

    constexpr explicit StrongType(ValueType && value) noexcept
        : m_value(std::move(value))
    {}

    constexpr ValueType & get() noexcept { return m_value; }
    constexpr ValueType const & get() const noexcept { return m_value; }

private: /* Fields: */

    ValueType m_value;

}; /* class StrongType */

#define SHAREMIND_STRONGTYPE_H_(Name,op) \
    struct StrongType ## Name ## Comparable { \
        template <typename T> \
        struct impl { \
            friend constexpr bool operator op(T const & a, T const & b) \
                    noexcept(noexcept(bool(a.get() op b.get()))) \
            { return bool(a.get() op b.get()); } \
        }; \
    };
#ifdef SHAREMIND_STRONGTYPE_H_TEST
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
SHAREMIND_STRONGTYPE_H_(Equality,==)
SHAREMIND_STRONGTYPE_H_(Inequality,!=)
#ifdef SHAREMIND_STRONGTYPE_H_TEST
#pragma GCC diagnostic pop
#endif
SHAREMIND_STRONGTYPE_H_(LessThan,<)
SHAREMIND_STRONGTYPE_H_(LessOrEqual,<=)
SHAREMIND_STRONGTYPE_H_(GreaterThan,>)
SHAREMIND_STRONGTYPE_H_(GreaterOrEqual,>=)
#undef SHAREMIND_STRONGTYPE_H_

struct StrongTypeBasicComparable {
    template <typename T>
    struct impl
            : StrongTypeEqualityComparable::impl<T>
            , StrongTypeInequalityComparable::impl<T>
    {};
};

using StrongTypeSortComparable = StrongTypeLessThanComparable;

struct StrongTypeFullyComparable {
    template <typename T>
    struct impl
            : StrongTypeBasicComparable::impl<T>
            , StrongTypeLessThanComparable::impl<T>
            , StrongTypeLessOrEqualComparable::impl<T>
            , StrongTypeGreaterThanComparable::impl<T>
            , StrongTypeGreaterOrEqualComparable::impl<T>
    {};
};

#define SHAREMIND_STRONGTYPE_H_(Name,op) \
    template <typename Other> \
    struct StrongType ## Name ## With { \
        template <typename T> \
        struct impl { \
            friend constexpr T & operator op(T & lhs, Other const & rhs) \
                    noexcept(noexcept(lhs.get() op rhs)) \
            { \
                lhs.get() op rhs; \
                return lhs; \
            } \
        }; \
    }; \
    template <typename Other, typename Tag, typename ... Mixins> \
    struct StrongType ## Name ## With<StrongType<Other, Tag, Mixins...> > { \
        template <typename T> \
        struct impl { \
            friend constexpr T & operator op( \
                    T & lhs, \
                    StrongType<Other, Tag, Mixins...> const & rhs) \
                    noexcept(noexcept(lhs.get() op rhs.get())) \
            { \
                lhs.get() op rhs.get(); \
                return lhs; \
            } \
        }; \
    }; \
    struct StrongType ## Name { \
        template <typename T> \
        using impl = typename StrongType ## Name ## With<T>::template impl<T>; \
    };
SHAREMIND_STRONGTYPE_H_(AddAssignable,+=)
SHAREMIND_STRONGTYPE_H_(SubAssignable,-=)
#if defined(SHAREMIND_STRONGTYPE_H_TEST) && defined(__GNUG__) \
    && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#endif
SHAREMIND_STRONGTYPE_H_(MulAssignable,*=)
#if defined(SHAREMIND_STRONGTYPE_H_TEST) && defined(__GNUG__) \
    && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
SHAREMIND_STRONGTYPE_H_(DivAssignable,/=)
SHAREMIND_STRONGTYPE_H_(ModAssignable,%=)
SHAREMIND_STRONGTYPE_H_(XorAssignable,^=)
SHAREMIND_STRONGTYPE_H_(AndAssignable,&=)
SHAREMIND_STRONGTYPE_H_(OrAssignable,|=)
#undef SHAREMIND_STRONGTYPE_H_

struct StrongTypePreIncrementable {
    template <typename T>
    struct impl {
        T & operator++() noexcept(noexcept(++(std::declval<T *>()->get()))) {
            auto & v = *static_cast<T *>(this);
            ++(v.get());
            return v;
        }
    };
};

struct StrongTypePostIncrementable {
    template <typename T>
    struct impl {
        T operator++(int) noexcept(
                noexcept((std::declval<T *>()->get())++)
                && std::is_nothrow_copy_constructible<T>::value
                && std::is_nothrow_move_constructible<T>::value)
        {
            auto & v = *static_cast<T *>(this);
            auto old(v);
            (v.get())++;
            return std::move(old);
        }
    };
};

struct StrongTypeIncrementable {
    template <typename T>
    struct impl
            : StrongTypePreIncrementable::impl<T>
            , StrongTypePostIncrementable::impl<T>
    {
        using StrongTypePreIncrementable::impl<T>::operator++;
        using StrongTypePostIncrementable::impl<T>::operator++;
    };
};

struct StrongTypePreDecrementable {
    template <typename T>
    struct impl {
        T & operator--() noexcept(noexcept(--(std::declval<T *>()->get()))) {
            auto & v = *static_cast<T *>(this);
            --(v.get());
            return v;
        }
    };
};

struct StrongTypePostDecrementable {
    template <typename T>
    struct impl {
        T operator--(int) noexcept(
                noexcept((std::declval<T *>()->get())--)
                && std::is_nothrow_copy_constructible<T>::value
                && std::is_nothrow_move_constructible<T>::value)
        {
            auto & v = *static_cast<T *>(this);
            auto old(v);
            (v.get())--;
            return std::move(old);
        }
    };
};

struct StrongTypeDecrementable {
    template <typename T>
    struct impl
            : StrongTypePreDecrementable::impl<T>
            , StrongTypePostDecrementable::impl<T>
    {
        using StrongTypePreDecrementable::impl<T>::operator--;
        using StrongTypePostDecrementable::impl<T>::operator--;
    };
};

template <typename Stream>
struct StrongTypeStreamableTo {
    template <typename T>
    struct impl {
        friend constexpr decltype(auto) operator<<(Stream & os, T const & value)
        { return os << value.get(); }
    };
};

using StrongTypeStreamable = StrongTypeStreamableTo<std::ostream>;

struct StrongTypeSwappable {
    template <typename T>
    struct impl {
        friend void swap(T & a, T & b) noexcept(IsNothrowSwappable<T>::value) {
            using std::swap;
            swap(a.value(), b.value());
        }
    };
};

template <typename T> struct IsStrongType : std::false_type {};

template <typename T, typename Tag, typename ... Mixins>
struct IsStrongType<StrongType<T, Tag, Mixins...> > : std::true_type {};

} /* namespace Sharemind { */

namespace std {

template <typename T, typename Tag, typename ... Mixins>
struct hash<
        sharemind::StrongType<
            T,
            Tag,
            sharemind::StrongTypeHashable,
            Mixins...
        >
    >
{
    size_t operator()(
            sharemind::StrongType<
                T, Tag, sharemind::StrongTypeHashable, Mixins...>
                const & v) const noexcept
    { return hash<sharemind::RemoveCvrefT<decltype(v.get())> >()(v.get()); }
};

} /* namespace std */
#endif /* SHAREMIND_STRONGTYPE_H */
