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

#include "../src/StringHashTablePredicate.h"

#include <boost/range/as_literal.hpp>
#include <boost/range/join.hpp>
#include <type_traits>
#include "../src/Range.h"
#include "../src/StringHasher.h"
#include "../src/TestAssert.h"


namespace {

#define TESTSTRING_PART1 "Hello, "
#define TESTSTRING_PART2 "World!"
#define TESTSTRING TESTSTRING_PART1 TESTSTRING_PART2

std::string const testString(TESTSTRING);
auto const testStringHash(sharemind::StringHasher()(testString));

// Workaround COW and similar optimizations just in case:
std::string const otherString(testString.c_str());

void testStringRefStringHashTablePredicate() {
    using P = sharemind::Detail::StringRefStringHashTablePredicate;
    static_assert(std::is_nothrow_copy_assignable<P>::value, "");
    static_assert(std::is_nothrow_copy_constructible<P>::value, "");
    static_assert(std::is_nothrow_move_assignable<P>::value, "");
    static_assert(std::is_nothrow_move_constructible<P>::value, "");

    P pred(otherString);
    SHAREMIND_TESTASSERT(pred.hash() == testStringHash);
    SHAREMIND_TESTASSERT(pred(testString));
}

void testCStringRefStringHashTablePredicate() {
    using P = sharemind::Detail::CStringRefStringHashTablePredicate;
    static_assert(std::is_nothrow_copy_assignable<P>::value, "");
    static_assert(std::is_nothrow_copy_constructible<P>::value, "");
    static_assert(std::is_nothrow_move_assignable<P>::value, "");
    static_assert(std::is_nothrow_move_constructible<P>::value, "");

    P pred(otherString.c_str());
    SHAREMIND_TESTASSERT(pred.hash() == testStringHash);
    SHAREMIND_TESTASSERT(pred(testString));
}

template <typename Range>
using RRSHTP = sharemind::Detail::RangeRefStringHashTablePredicate<Range>;

void testRangeRefStringHashTablePredicate() {
    #define RANGETEST(type,...) \
        do { \
            using P = RRSHTP<type>; \
            static_assert(std::is_nothrow_copy_constructible<P>::value, ""); \
            static_assert(std::is_nothrow_move_constructible<P>::value, ""); \
            P pred(__VA_ARGS__); \
            SHAREMIND_TESTASSERT(pred.hash() == testStringHash); \
            SHAREMIND_TESTASSERT(pred(testString)); \
        } while(false)

    RANGETEST(std::string const, otherString);
    {
        auto const r(boost::join(boost::as_literal(TESTSTRING_PART1),
                                 boost::as_literal(TESTSTRING_PART2)));
        RANGETEST(decltype(r), r);
    }
    //RANGETEST(decltype(boost::as_literal(TESTSTRING)),
    //          boost::as_literal(TESTSTRING));
}

template <typename Expected, typename T>
void testGetOrCreate(T && t, bool expectToSucceed = true) {
    auto pred(sharemind::getOrCreateTemporaryStringHashTablePredicate(
                  std::forward<T>(t)));
    using P = decltype(pred);
    static_assert(std::is_same<P, Expected>::value, "");
    static_assert(std::is_nothrow_copy_constructible<P>::value, "");
    static_assert(std::is_nothrow_move_constructible<P>::value, "");

    SHAREMIND_TESTASSERT(pred.hash() == testStringHash);
    SHAREMIND_TESTASSERT(pred(testString) ? expectToSucceed : !expectToSucceed);
}

} // anonymous namespace

int main() {
    testStringRefStringHashTablePredicate();
    testCStringRefStringHashTablePredicate();
    testRangeRefStringHashTablePredicate();

    static std::string const regularString(TESTSTRING);
    using S = sharemind::Detail::StringRefStringHashTablePredicate;
    testGetOrCreate<S>(regularString);

    static char const * const nullTerminatedString = TESTSTRING;
    using C = sharemind::Detail::CStringRefStringHashTablePredicate;
    testGetOrCreate<C>(nullTerminatedString);

    testGetOrCreate<RRSHTP<decltype(boost::as_literal(TESTSTRING))> >(
                boost::as_literal(TESTSTRING));
    testGetOrCreate<
            RRSHTP<decltype(sharemind::asLiteralStringRange(TESTSTRING))> >(
                sharemind::asLiteralStringRange(TESTSTRING));
    testGetOrCreate<RRSHTP<decltype(TESTSTRING)> >(TESTSTRING, false);
}
