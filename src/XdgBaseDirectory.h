/*
 * Copyright (C) 2017 Cybernetica
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

#ifndef SHAREMIND_XDGBASEDIRECTORY_H
#define SHAREMIND_XDGBASEDIRECTORY_H

/**
  \file Confirming to XDG Base Directory Specification version 0.6,
        https://specifications.freedesktop.org/basedir-spec/basedir-spec-0.6.html
*/

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "HomeDirectory.h"
#include "Split.h"


namespace sharemind {

inline std::string getDefaultXdgDataHome()
{ return getHomeDirectory() + "/.local/share"; }

inline std::string getDefaultXdgConfigHome()
{ return getHomeDirectory() + "/.config"; }

inline std::vector<std::string> const & getDefaultXdgDataDirs() {
    static std::vector<std::string> const r{std::string("/usr/local/share/"),
                                            std::string("/usr/share/")};
    return r;
}

inline std::vector<std::string> const & getDefaultXdgConfigDirs() {
    static std::vector<std::string> const r{std::string("/etc/xdg/")};
    return r;
}

inline std::string getDefaultXdgCacheHome()
{ return getHomeDirectory() + "/.cache"; }

namespace Detail {
namespace Xdg {

template <typename DefaultGenerator>
inline std::string getDir(char const * const envVarName,
                          DefaultGenerator defaultGenerator)
{
    auto const * const e = std::getenv(envVarName);
    if (!e || !*e)
        return defaultGenerator();
    return e;
}

template <typename DefaultGenerator>
inline std::vector<std::string> getDirs(char const * const envVarName,
                                        DefaultGenerator defaultGenerator)
{
    auto const * const e = std::getenv(envVarName);
    if (!e || !*e)
        return defaultGenerator();
    auto const * const end = e + std::strlen(e);
    std::vector<std::string> r;
    split(e,
          end,
          [](char const & c) noexcept { return c == ':'; },
          [&r](char const * const begin_, char const * const end_) noexcept
          { r.emplace_back(begin_, end_); });
    return r;
}

} /* namespace Xdg { */
} /* namespace Detail { */

/** \returns $XDG_DATA_HOME */
inline std::string getXdgDataHome()
{ return Detail::Xdg::getDir("XDG_DATA_HOME", &getDefaultXdgDataHome); }

/** \returns $XDG_CONFIG_HOME */
inline std::string getXdgConfigHome()
{ return Detail::Xdg::getDir("XDG_CONFIG_HOME", &getDefaultXdgConfigHome); }

/** \returns $XDG_DATA_DIRS */
inline std::vector<std::string> getXdgDataDirs()
{ return Detail::Xdg::getDirs("XDG_DATA_DIRS", &getDefaultXdgDataDirs); }

/** \returns $XDG_CONFIG_DIRS */
inline std::vector<std::string> getXdgConfigDirs()
{ return Detail::Xdg::getDirs("XDG_CONFIG_DIRS", &getDefaultXdgConfigDirs); }

/** \returns $XDG_CACHE_HOME */
inline std::string getXdgCacheHome()
{ return Detail::Xdg::getDir("XDG_CACHE_HOME", &getDefaultXdgCacheHome); }

/**
    \brief Shorthand to retrieve all configuration paths combined with the given
           suffix.
    \arg[in] suffix Suffix to append to each path. Should be either empty or
                    begin with a directory separator.
    \returns a vector of configuration paths containing the value of
             getXdgConfigHome() and all values from getXdgConfigDirs() in order,
             with the given suffix appended to each element.
*/
inline std::vector<std::string> getXdgConfigPaths(
        std::string const & suffix = std::string())
{
    assert(suffix.empty() || suffix[0u] == '/');
    std::vector<std::string> r;
    r.emplace_back(getXdgConfigHome() + suffix);
    for (auto & path : getXdgConfigDirs())
        r.emplace_back(std::move(path) + suffix);
    return r;
}

/**
    \brief Shorthand to retrieve all data paths combined with the given suffix.
    \arg[in] suffix Suffix to append to each path. Should be either empty or
                    begin with a directory separator.
    \returns a vector of data paths containing the value of getXdgDataHome() and
             all values from getXdgDataDirs() in order, with the given suffix
             appended to each element.
*/
inline std::vector<std::string> getXdgDataPaths(
        std::string const & suffix = std::string())
{
    assert(suffix.empty() || suffix[0u] == '/');
    std::vector<std::string> r;
    r.emplace_back(getXdgDataHome() + suffix);
    for (auto & path : getXdgDataDirs())
        r.emplace_back(std::move(path) + suffix);
    return r;
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_XDGBASEDIRECTORY_H */
