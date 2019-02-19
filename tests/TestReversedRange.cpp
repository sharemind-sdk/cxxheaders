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

#include "../src/ReversedRange.h"

#include <algorithm>
#include <vector>
#include "../src/TestAssert.h"


int main() {
    std::vector<int> const v{{0,1,2,3,4}};
    std::vector<int> r;
    r.reserve(v.size());
    for (auto const & i : sharemind::reverseRange(v))
        r.emplace_back(i);
    std::reverse(r.begin(), r.end());
    SHAREMIND_TESTASSERT(v == r);
    std::reverse(r.begin(), r.end());

    auto rrw = sharemind::reverseRange(v);
    auto rit = v.crbegin();
    for (auto it = std::cbegin(rrw); it != std::cend(rrw); ++it, ++rit)
        SHAREMIND_TESTASSERT(*it == *rit);
}
