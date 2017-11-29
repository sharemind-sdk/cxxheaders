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

#ifndef SHAREMIND_CONCAT_H
#define SHAREMIND_CONCAT_H

#include <iostream>
#include <sstream>
#include <string>
#include <utility>


namespace sharemind {
namespace Detail {

template <typename Arg>
inline std::string concatBase(std::ostringstream && oss, Arg && arg) {
    oss << std::forward<Arg>(arg);
    return oss.str();
}

template <typename Arg, typename ... Args>
inline std::string concatBase(std::ostringstream && oss,
                              Arg && arg,
                              Args && ... args)
{
    oss << std::move(arg);
    return concatBase(std::move(oss), std::forward<Args>(args)...);
}

} /* namespace Detail { */

inline std::string concat() { return std::string(); }
inline std::string concat(const char * const s) { return s; }
inline std::string concat(std::string && s) { return std::move(s); }
inline std::string concat(const std::string & s) { return s; }

template <typename ... T>
inline std::string concat(T && ... args)
{ return Detail::concatBase(std::ostringstream(), std::forward<T>(args)...); }

template <typename ... Args>
inline std::basic_ostream<std::ostringstream::char_type,
                          std::ostringstream::traits_type> &
concat_endl(Args && ... args)
    noexcept(noexcept(
                 std::endl<std::ostringstream::char_type,
                           std::ostringstream::traits_type>(
                        std::forward<Args>(args)...)))
{
    return std::endl<std::ostringstream::char_type,
                     std::ostringstream::traits_type>(
                std::forward<Args>(args)...);
}

} /* namespace sharemind { */

#endif /* SHAREMIND_CONCAT_H */
