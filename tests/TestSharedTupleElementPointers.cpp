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

#include "../src/SharedTupleElementPointers.h"

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

template <typename ... Ts> using T = std::tuple<Ts...>;
template <typename T> using S = std::shared_ptr<T>;
using E0 = int;
using E1 = int *;
using E2 = std::string;

template <typename X, typename Y>
inline bool sameSharedPtrGroup(S<X> const & x, S<Y> const & y)
{ return !x.owner_before(y) && !y.owner_before(x); }

int main() {
    int x = 42;
    auto packed(std::make_shared<T<E0, E1, E2> >(13, &x, "Hello, World!"));

    // Test unpackSharedTuple():
    assert(packed.unique()); // obvious
    assert(packed.use_count() == 1); // obvious
    {
        auto unpacked(sharemind::makeAllSharedTupleElementPtrs(packed));
        static_assert(std::is_same<decltype(unpacked),
                                   T<S<E0>, S<E1>, S<E2> > >::value, "");
        assert(!packed.unique());
        assert(packed.use_count() == 4);
        assert(sameSharedPtrGroup(std::get<0u>(unpacked), packed));
        assert(sameSharedPtrGroup(std::get<1u>(unpacked), packed));
        assert(sameSharedPtrGroup(std::get<2u>(unpacked), packed));
        assert(std::get<0u>(unpacked).get() == &std::get<0u>(*packed));
        assert(std::get<1u>(unpacked).get() == &std::get<1u>(*packed));
        assert(std::get<2u>(unpacked).get() == &std::get<2u>(*packed));
    }

    // Test partiallyUnpackSharedTuple():
    assert(packed.unique()); // obvious
    assert(packed.use_count() == 1); // obvious
    {
        auto unpacked(sharemind::makeSharedTupleElementPtrs<1u, 0u>(packed));
        static_assert(std::is_same<decltype(unpacked),
                                   T<S<E1>, S<E0> > >::value, "");
        assert(!packed.unique());
        assert(packed.use_count() == 3);
        assert(sameSharedPtrGroup(std::get<0u>(unpacked), packed));
        assert(sameSharedPtrGroup(std::get<1u>(unpacked), packed));
        assert(std::get<1u>(unpacked).get() == &std::get<0u>(*packed));
        assert(std::get<0u>(unpacked).get() == &std::get<1u>(*packed));
    }

    // Test extractSharedTupleElement():
    assert(packed.unique()); // obvious
    assert(packed.use_count() == 1); // obvious
    {
        auto element(sharemind::makeSharedTupleElementPtr<2u>(packed));
        static_assert(std::is_same<decltype(element),
                                   std::shared_ptr<std::string> >::value, "");
        assert(!packed.unique());
        assert(packed.use_count() == 2);
        assert(sameSharedPtrGroup(element, packed));
        assert(element.get() == &std::get<2u>(*packed));
    }
}
