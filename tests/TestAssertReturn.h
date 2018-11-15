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

#include "../src/AssertReturn.h"

#include <cstdlib>
#include <memory>
#include "../src/TestAssert.h"
#ifndef NDEBUG
#include <signal.h>
#endif
#include <type_traits>


struct A { explicit operator bool() const volatile noexcept; };
struct B { explicit operator bool() const volatile; };
#define SAME(T) \
    std::is_same<decltype(sharemind::assertReturn(std::declval<T>())), \
                 decltype(std::declval<T>())>::value
static_assert(SAME(A), "");
static_assert(SAME(A const), "");
static_assert(SAME(A volatile), "");
static_assert(SAME(A const volatile), "");
static_assert(SAME(A &), "");
static_assert(SAME(A const &), "");
static_assert(SAME(A volatile &), "");
static_assert(SAME(A const volatile &), "");
static_assert(SAME(A &&), "");
static_assert(SAME(A const &&), "");
static_assert(SAME(A volatile &&), "");
static_assert(SAME(A const volatile &&), "");
static_assert(noexcept(sharemind::assertReturn(std::declval<A>())), "");
#ifndef NDEBUG
static_assert(!noexcept(sharemind::assertReturn(std::declval<B>())), "");
#else
static_assert(noexcept(sharemind::assertReturn(std::declval<B>())), "");
#endif


#ifndef NDEBUG
[[noreturn]] void handleAbort(int);
void handleAbort(int) { exit(0); }
#endif


int main() {
    auto sPtr = std::make_shared<int>(42);
    std::weak_ptr<int> wPtr(sPtr);
    auto sPtr2 = sharemind::assertReturn(wPtr.lock());
    SHAREMIND_TESTASSERT(sPtr2.use_count() == 2u);

    sPtr.reset();
    sPtr2.reset();

    #ifndef NDEBUG
    signal(SIGABRT, &handleAbort);
    #endif

    auto const sPtr3 = sharemind::assertReturn(wPtr.lock());
    SHAREMIND_TESTASSERT(!sPtr3);
    #ifndef NDEBUG
    return EXIT_FAILURE;
    #endif
}
