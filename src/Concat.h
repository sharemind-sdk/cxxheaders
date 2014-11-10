/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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

const constexpr auto concat_endl = &std::endl<std::ostringstream::char_type,
                                              std::ostringstream::traits_type>;

} /* namespace sharemind { */

#endif /* SHAREMIND_CONCAT_H */
