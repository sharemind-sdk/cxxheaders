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

#ifndef SHAREMIND_ADD_H
#define SHAREMIND_ADD_H

#include <utility>


namespace sharemind {
namespace Detail {
namespace Add {

template <typename T, typename ... Ts> struct Adder;

template <typename T>
struct Adder<T>
{ static constexpr T const & add(T const & a) noexcept { return a; } };

template <typename A, typename B>
struct Adder<A, B> {
    static constexpr auto add(A && a, B && b) noexcept ->
            decltype(std::forward<A>(a) + std::forward<B>(b))
    { return std::forward<A>(a) + std::forward<B>(b); }
};

template <typename A, typename B, typename ... Args>
struct Adder<A, B, Args...> {
    static constexpr auto add(A && a,
                              B && b,
                              Args && ... args) noexcept ->
            decltype(
                Adder<
                    decltype(Adder<A, B>::add(std::forward<A>(a),
                                              std::forward<B>(b))),
                    Args...
                >::add(
                    Adder<A, B>::add(std::forward<A>(a), std::forward<B>(b)),
                    std::forward<Args>(args)...))
    {
        return Adder<
                decltype(Adder<A, B>::add(std::forward<A>(a),
                                          std::forward<B>(b))),
                Args...
            >::add(
                Adder<A, B>::add(std::forward<A>(a), std::forward<B>(b)),
                std::forward<Args>(args)...);
    }
};

} /* namespace Add { */
} /* namespace Detail { */

template <typename Arg, typename ... Args>
constexpr auto add(Arg && arg, Args && ... args) noexcept ->
        decltype(Detail::Add::Adder<Arg, Args...>::add(
                     std::forward<Arg>(arg),
                     std::forward<Args>(args)...))
{
    return Detail::Add::Adder<Arg, Args...>::add(std::forward<Arg>(arg),
                                                 std::forward<Args>(args)...);
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_MAX_H */
