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

#include <cstddef>
#include <memory>
#include <utility>
#include "../src/AssertReturn.h"
#include "../src/TestAssert.h"
#include "../src/Detected.h"


using sharemind::makeUnique;
using sharemind::IsDetected;
using sharemind::IsDetectedExact;

template <typename T> using U = std::unique_ptr<T>;

struct V {};
struct X {
    X() noexcept {}
    explicit X(V * const v) noexcept : m_v(v) {}
    V * const m_v = nullptr;
};
struct Y;
template <typename T> struct Z;



template <typename T, typename ... Args>
using CanMakeUnique = decltype(makeUnique<T>(std::declval<Args>()...));

// No arguments:
static_assert(IsDetected<CanMakeUnique, X>::value, "");
static_assert(IsDetectedExact<U<X>, CanMakeUnique, X>::value, "");
static_assert(!IsDetected<CanMakeUnique, X[]>::value, "");
static_assert(!IsDetected<CanMakeUnique, X[42]>::value, "");

// Random (Y *, Z<Y> const &) arguments:
static_assert(IsDetected<CanMakeUnique, X, Y *, Z<Y> const &>::value, "");
static_assert(IsDetectedExact<U<X>, CanMakeUnique, X, Y *, Z<Y> const &>::value,
              "");
static_assert(!IsDetected<CanMakeUnique, X[], Y *, Z<Y> const &>::value, "");
static_assert(!IsDetected<CanMakeUnique, X[42], Y *, Z<Y> const &>::value, "");

// Size arguments:
static_assert(IsDetected<CanMakeUnique, X, std::size_t>::value, "");
static_assert(IsDetectedExact<U<X>, CanMakeUnique, X, std::size_t>::value, "");
static_assert(IsDetected<CanMakeUnique, X[], std::size_t>::value, "");
static_assert(IsDetectedExact<U<X[]>, CanMakeUnique, X[], std::size_t>::value,
              "");
static_assert(!IsDetected<CanMakeUnique, X[42], std::size_t>::value, "");

// Size and random (Y *, Z<Y> const &) arguments:
static_assert(
        IsDetected<CanMakeUnique, X, std::size_t, Y *, Z<Y> const &>::value,"");
static_assert(
        IsDetectedExact<U<X>, CanMakeUnique, X, std::size_t, Y *, Z<Y> const &>
            ::value, "");
static_assert(
        !IsDetected<CanMakeUnique, X[], std::size_t, Y *, Z<Y> const &>::value,
        "");
static_assert(
        !IsDetected<CanMakeUnique, X[42], std::size_t, Y *, Z<Y> const &>
            ::value, "");

int main() {
    V v;
    SHAREMIND_TESTASSERT(!sharemind::assertReturn(makeUnique<X>())->m_v);
    SHAREMIND_TESTASSERT(sharemind::assertReturn(makeUnique<X>(&v))->m_v == &v);
    auto const ptr(sharemind::assertReturn(makeUnique<X[]>(42)));
    for (unsigned i = 0u; i < 42u; ++i)
        SHAREMIND_TESTASSERT(!ptr[i].m_v);
}
