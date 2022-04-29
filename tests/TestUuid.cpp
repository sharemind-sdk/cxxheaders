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

#include "../src/Uuid.h"

#include <algorithm>
#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include <cstdint>
#include "../src/TestAssert.h"


int main() {
    using namespace sharemind;
    Uuid u { {0} };
    SHAREMIND_TESTASSERT(std::all_of(u.begin(), u.end(),
                                     [](std::uint8_t x){ return x == 0; }));
    constexpr const char* zeroUuidStr = "00000000-0000-0000-0000-000000000000";
    SHAREMIND_TESTASSERT(uuidToString(u) == zeroUuidStr);
    SHAREMIND_TESTASSERT(uuidFromString(uuidToString(sharemindNamespaceUuid)) ==
                         sharemindNamespaceUuid);
    SHAREMIND_TESTASSERT(uuidFromString(uuidToString(u)) == u);
    SHAREMIND_TESTASSERT(u != sharemindNamespaceUuid);
    SHAREMIND_TESTASSERT(uuidToString(u) !=
                         uuidToString(sharemindNamespaceUuid));

    try {
        u = uuidFromString("asd");
    } catch (boost::bad_lexical_cast const &) {
        return 0;
    }
    SHAREMIND_TEST_UNREACHABLE;
}
