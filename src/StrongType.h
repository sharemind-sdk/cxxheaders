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
        auto hash() const {
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

    static_assert(Detail::StrongTypeHashableCheck<Mixins...>::value,
                  "StrongTypeHashable needs to be the first mixin to work!");

public: /* Types: */

    using ValueType = T;

public: /* Methods: */

    constexpr explicit StrongType(ValueType const & value)
            noexcept(std::is_nothrow_copy_constructible<T>::value)
        : m_value(std::move(value))
    {}

    constexpr explicit StrongType(ValueType && value)
            noexcept(std::is_nothrow_move_constructible<T>::value)
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
SHAREMIND_STRONGTYPE_H_(Equality,==)
SHAREMIND_STRONGTYPE_H_(Inequality,!=)
SHAREMIND_STRONGTYPE_H_(LessThan,<)
SHAREMIND_STRONGTYPE_H_(LessOrEqual,<=)
SHAREMIND_STRONGTYPE_H_(GreaterThan,>)
SHAREMIND_STRONGTYPE_H_(GreaterOrEqual,>=)
#undef SHAREMIND_STRONGTYPE_H_

struct StrongTypeRegularComparable {
    template <typename T>
    struct impl
            : StrongTypeEqualityComparable::impl<T>
            , StrongTypeInequalityComparable::impl<T>
    {};
};

struct StrongTypeSortComparable {
    template <typename T>
    struct impl
            : StrongTypeRegularComparable::impl<T>
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
            friend constexpr bool operator op(T & lhs, Other const & rhs) \
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
            friend constexpr bool operator op( \
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
        using impl = StrongType ## Name ## With<T>; \
    };
SHAREMIND_STRONGTYPE_H_(AddAssignable,+=)
SHAREMIND_STRONGTYPE_H_(SubAssignable,-=)
SHAREMIND_STRONGTYPE_H_(MulAssignable,*=)
SHAREMIND_STRONGTYPE_H_(DivAssignable,/=)
SHAREMIND_STRONGTYPE_H_(ModAssignable,%=)
SHAREMIND_STRONGTYPE_H_(XorAssignable,^=)
SHAREMIND_STRONGTYPE_H_(AndAssignable,&=)
SHAREMIND_STRONGTYPE_H_(OrAssignable,|=)
#undef SHAREMIND_STRONGTYPE_H_

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