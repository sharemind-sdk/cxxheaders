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

#include "../src/StringHasher.h"

#include "../src/TestAssert.h"
#include <cstddef>
#include <string>
#include <type_traits>


namespace {

using H = sharemind::StringHasher;

static char const staticString[] = "TestStringHasher";
char buf1[]                      = "TestStringHasher";
char buf2[]                      = "TestStringHasher";
char buf3[]                      = "TestStringHasher";
char buf4[]                      = "TestStringHasher";
static char const * const cStyleString1 = buf1;
static char const * cStyleString2 = buf2;
static char * const cStyleString4 = buf3;
static char * cStyleString3 = buf4;
std::string stdString(staticString);
std::string const stdStringC(stdString);
std::size_t const expected = H()(staticString);

} // anonymous namespace

#define TEST_ATOM(input,...) \
    do { \
        static_assert(std::is_same<std::size_t, \
                                   decltype(__VA_ARGS__(input))>::value, ""); \
        SHAREMIND_TESTASSERT((__VA_ARGS__(input)) == expected); \
    } while (false)
#define TEST_ONE(...) \
    TEST_ATOM(staticString, __VA_ARGS__); \
    TEST_ATOM(cStyleString1, __VA_ARGS__); \
    TEST_ATOM(cStyleString2, __VA_ARGS__); \
    TEST_ATOM(cStyleString3, __VA_ARGS__); \
    TEST_ATOM(cStyleString4, __VA_ARGS__); \
    TEST_ATOM(stdString, __VA_ARGS__); \
    TEST_ATOM(stdStringC, __VA_ARGS__)


int main() {
    TEST_ONE(H());
    H h;
    TEST_ONE(h);
    H h2;
    TEST_ONE(h2);
    H const hc;
    TEST_ONE(hc);
    H const hc2;
    TEST_ONE(hc2);
}
