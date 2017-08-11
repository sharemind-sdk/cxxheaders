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

#include "../src/XdgBaseDirectory.h"

#include <cassert>
#include <stdlib.h>
#include <type_traits>
#include "../src/HomeDirectory.h"
#include "../src/TestAssert.h"


using S = std::string;
using SV = std::vector<S>;

#define TEST_RETURNS_TYPE(type,...) \
    static_assert(std::is_same<type, decltype(__VA_ARGS__)>::value, "")
#define TEST_RETURNS_STRING(...) TEST_RETURNS_TYPE(S, __VA_ARGS__)
#define TEST_RETURNS_STRING_VECTOR(...) TEST_RETURNS_TYPE(SV, __VA_ARGS__)
TEST_RETURNS_STRING(sharemind::getDefaultXdgDataHome());
TEST_RETURNS_STRING(sharemind::getDefaultXdgConfigHome());
TEST_RETURNS_TYPE(SV const &, sharemind::getDefaultXdgDataDirs());
TEST_RETURNS_TYPE(SV const &, sharemind::getDefaultXdgConfigDirs());
TEST_RETURNS_STRING(sharemind::getDefaultXdgCacheHome());
TEST_RETURNS_STRING(sharemind::getXdgDataHome());
TEST_RETURNS_STRING(sharemind::getXdgConfigHome());
TEST_RETURNS_STRING_VECTOR(sharemind::getXdgDataDirs());
TEST_RETURNS_STRING_VECTOR(sharemind::getXdgConfigDirs());
TEST_RETURNS_STRING(sharemind::getXdgCacheHome());

int main() {
    static auto const resetXdgEnvironment =
            [] {
                ::unsetenv("XDG_DATA_HOME");
                ::unsetenv("XDG_CONFIG_HOME");
                ::unsetenv("XDG_DATA_DIRS");
                ::unsetenv("XDG_CONFIG_DIRS");
                ::unsetenv("XDG_CACHE_HOME");
            };

    static auto const testDefaults =
            []() {
                auto const defDataHome(sharemind::getDefaultXdgDataHome());
                SHAREMIND_TESTASSERT(
                            defDataHome
                            == sharemind::getHomeDirectory() + "/.local/share");

                auto const defConfigHome(sharemind::getDefaultXdgConfigHome());
                SHAREMIND_TESTASSERT(
                            defConfigHome
                            == sharemind::getHomeDirectory() + "/.config");

                auto const defDataDirs(sharemind::getDefaultXdgDataDirs());
                SHAREMIND_TESTASSERT(defDataDirs.size() == 2u);
                SHAREMIND_TESTASSERT(defDataDirs[0u] == "/usr/local/share/");
                SHAREMIND_TESTASSERT(defDataDirs[1u] == "/usr/share/");

                auto const defConfigDirs(sharemind::getDefaultXdgConfigDirs());
                SHAREMIND_TESTASSERT(defConfigDirs.size() == 1u);
                SHAREMIND_TESTASSERT(defConfigDirs[0u] == "/etc/xdg/");

                auto const defCacheHome(sharemind::getDefaultXdgCacheHome());
                SHAREMIND_TESTASSERT(
                            sharemind::getDefaultXdgCacheHome()
                            == sharemind::getHomeDirectory() + "/.cache");
            };
    testDefaults();
    ::setenv("HOME", "/sharemindTests", 1);
    testDefaults();

    #define TEST_FALLBACK(...) \
        SHAREMIND_TESTASSERT(sharemind::getXdg ## __VA_ARGS__() \
                             == sharemind::getDefaultXdg ## __VA_ARGS__());
    static auto const testDefaultFallbacks =
            [&]() {
                TEST_FALLBACK(DataHome);
                TEST_FALLBACK(ConfigHome);
                TEST_FALLBACK(DataDirs);
                TEST_FALLBACK(ConfigDirs);
                TEST_FALLBACK(CacheHome);
            };
    testDefaultFallbacks();
    static auto const envVars = { "XDG_DATA_HOME", "XDG_CONFIG_HOME",
                                  "XDG_DATA_DIRS", "XDG_CONFIG_DIRS",
                                  "XDG_CACHE_HOME" };
    for (auto const & envVar : envVars) {
        resetXdgEnvironment();
        ::setenv(envVar, "", 1);
        testDefaultFallbacks();
    }
    resetXdgEnvironment();

    ::setenv("XDG_DATA_HOME", "/asdf", 1);
    SHAREMIND_TESTASSERT(sharemind::getXdgDataHome() == "/asdf");
    resetXdgEnvironment();

    ::setenv("XDG_CONFIG_HOME", "/asdf", 1);
    SHAREMIND_TESTASSERT(sharemind::getXdgConfigHome() == "/asdf");
    resetXdgEnvironment();

    ::setenv("XDG_DATA_DIRS", ":/asdf::/other:", 1);
    SHAREMIND_TESTASSERT(sharemind::getXdgDataDirs()
                         == SV{S(), S("/asdf"), S(), S("/other"), S()});
    resetXdgEnvironment();

    ::setenv("XDG_CONFIG_DIRS", ":/asdf::/other:", 1);
    SHAREMIND_TESTASSERT(sharemind::getXdgConfigDirs()
                         == SV{S(), S("/asdf"), S(), S("/other"), S()});
    resetXdgEnvironment();

    ::setenv("XDG_CACHE_HOME", "/asdf", 1);
    SHAREMIND_TESTASSERT(sharemind::getXdgCacheHome() == "/asdf");
    resetXdgEnvironment();

    ::setenv("XDG_CONFIG_HOME", "/conf", 1);
    ::setenv("XDG_CONFIG_DIRS", "/conf2:/otherconf", 1);
    {
        SV expected;
        expected.emplace_back("/conf/asdf");
        expected.emplace_back("/conf2/asdf");
        expected.emplace_back("/otherconf/asdf");
        SHAREMIND_TESTASSERT(sharemind::getXdgConfigPaths("/asdf") == expected);
    }
    resetXdgEnvironment();

    ::setenv("XDG_DATA_HOME", "/data", 1);
    ::setenv("XDG_DATA_DIRS", "/data2:/otherdata", 1);
    {
        SV expected;
        expected.emplace_back("/data/asdf");
        expected.emplace_back("/data2/asdf");
        expected.emplace_back("/otherdata/asdf");
        SHAREMIND_TESTASSERT(sharemind::getXdgDataPaths("/asdf") == expected);
    }
    resetXdgEnvironment();


    ::setenv("XDG_CACHE_HOME", "/asdf", 1);
}
