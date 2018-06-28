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

#include "../src/ReversionWrapper.h"

#include <vector>
#include "../src/TestAssert.h"


int main() {
    using V = std::vector<int>;
    V v{1, 2, 3, 4, 5};
    auto const & cv = v;

    V const expected{5, 4, 3, 2, 1};

    {
        V r;
        for (auto const & rv : sharemind::makeReversionWrapper(v) )
            r.emplace_back(rv);
        SHAREMIND_TESTASSERT(r == expected);
    }

    {
        V r;
        for (auto const & rv : sharemind::makeReversionWrapper(cv) )
            r.emplace_back(rv);
        SHAREMIND_TESTASSERT(r == expected);
    }

    { // Test mutation:
        int i = 0;
        for (auto & rv : sharemind::makeReversionWrapper(v) )
            rv = ++i;
        SHAREMIND_TESTASSERT(v == expected);
    }
}
