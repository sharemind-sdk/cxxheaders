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

#include "../src/UnpackSharedTuple.h"

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>


using Packed = std::tuple<int, int *, std::string>;
using Unpacked =
    std::tuple<
        std::shared_ptr<int>,
        std::shared_ptr<int *>,
        std::shared_ptr<std::string>
    >;

template <typename X, typename Y>
inline bool sameSharedPtrGroup(std::shared_ptr<X> const & x,
                               std::shared_ptr<Y> const & y)
{ return !x.owner_before(y) && !y.owner_before(x); }

int main() {
    int x = 42;
    auto packed(std::make_shared<Packed>(13, &x, "Hello, World!"));
    assert(packed.unique()); // obvious
    assert(packed.use_count() == 1); // obvious
    auto unpacked(sharemind::unpackSharedTuple(packed));
    static_assert(std::is_same<decltype(unpacked), Unpacked>::value, "");
    assert(!packed.unique());
    assert(packed.use_count() == 4);
    assert(sameSharedPtrGroup(std::get<0u>(unpacked), packed));
    assert(sameSharedPtrGroup(std::get<1u>(unpacked), packed));
    assert(sameSharedPtrGroup(std::get<2u>(unpacked), packed));
    assert(std::get<0u>(unpacked).get() == &std::get<0u>(*packed));
    assert(std::get<1u>(unpacked).get() == &std::get<1u>(*packed));
    assert(std::get<2u>(unpacked).get() == &std::get<2u>(*packed));

    std::swap(*(std::get<0u>(unpacked)), **(std::get<1u>(unpacked)));
    assert(42 == std::get<0u>(*packed));
    assert(x == 13);
}
