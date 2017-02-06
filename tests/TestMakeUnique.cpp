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

#include "../src/MakeUnique.h"

#include <memory>
#include <type_traits>
#include "../src/AssertReturn.h"
#include "../src/TestAssert.h"
#include "../src/Void.h"


using sharemind::makeUnique;
using sharemind::Void_t;

struct X {
    static constexpr int const DEFAULT = 8;
    X(int const v = DEFAULT) noexcept : m_v(v) {}
    int m_v;
};

static_assert(std::is_same<decltype(makeUnique<X>()),
                           std::unique_ptr<X> >::value, "");
static_assert(std::is_same<decltype(makeUnique<X>(42)),
                           std::unique_ptr<X> >::value, "");
static_assert(std::is_same<decltype(makeUnique<X[]>(42)),
                           std::unique_ptr<X[]> >::value, "");

using False = std::integral_constant<bool, false>;
using True = std::integral_constant<bool, true>;

template <typename T, class = void> struct CanMakeUnique: False {};
template <typename T>
struct CanMakeUnique<T, Void_t<decltype(makeUnique<T>())> >: True {};
static_assert(CanMakeUnique<X>::value, "");
// static_assert(CanMakeUnique<X[]>::value, ""); // Needs arg
static_assert(!CanMakeUnique<X[42]>::value, "");

template <typename T, class = void> struct CanMakeUniqueWithArg: False {};
template <typename T>
struct CanMakeUniqueWithArg<T, Void_t<decltype(makeUnique<T>(42))> > : True {};
static_assert(CanMakeUniqueWithArg<X>::value, "");
static_assert(CanMakeUniqueWithArg<X[]>::value, "");
static_assert(!CanMakeUniqueWithArg<X[42]>::value, "");

int main() {
    SHAREMIND_TESTASSERT(
                SHAREMIND_ASSERTRETURN(makeUnique<X>())->m_v == X::DEFAULT);
    SHAREMIND_TESTASSERT(
                SHAREMIND_ASSERTRETURN(makeUnique<X>(42))->m_v == 42);
    auto const ptr(SHAREMIND_ASSERTRETURN(makeUnique<X[]>(42)));
    for (unsigned i = 0u; i < 42u; ++i)
        SHAREMIND_TESTASSERT(ptr[i].m_v == X::DEFAULT);
}
