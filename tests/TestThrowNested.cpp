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

#include "../src/ThrowNested.h"

#include <sharemind/compiler-support/GccVersion.h>
#include "../src/TestAssert.h"


using sharemind::throwNested;

/* Workaround GCC PR 62154 (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=62154):

  Sample error message:

 /usr/include/c++/4.9/bits/nested_exception.h:90:59: error: cannot dynamic_cast
        ‘& __ex’ (of type ‘const struct A*’) to type ‘const class
        std::nested_exception*’ (source type is not polymorphic)
       { return dynamic_cast<const nested_exception*>(&__ex); }
                                                           ^

*/
#if !defined(SHAREMIND_GCC_VERSION) || SHAREMIND_GCC_VERSION >= 50000
using FirstException = int;
#else
struct FirstException {
    FirstException(int v) noexcept : m_v(v) {}
    virtual ~FirstException() noexcept {}
    bool operator==(int v) const noexcept { return m_v == v; }
    int m_v;
};
#endif
struct A {};
struct B {};
struct C {};

int main() {
    try {
        throwNested(int(42));
    } catch (int const e) {
        SHAREMIND_TESTASSERT(e == 42);
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }

    try {
        throwNested(int(42), A());
    } catch (A const & e) {
        try {
            std::rethrow_if_nested(e);
        } catch (int const e) {
            SHAREMIND_TESTASSERT(e == 42);
        } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }

    try {
        throwNested(int(42), A(), B());
    } catch (B const & e) {
        try {
            std::rethrow_if_nested(e);
        } catch (A const & e) {
            try {
                std::rethrow_if_nested(e);
            } catch (int const e) {
                SHAREMIND_TESTASSERT(e == 42);
            } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
        } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }

    try {
        throwNested(int(42), A(), B(), C());
    } catch (C const & e) {
        try {
            std::rethrow_if_nested(e);
        } catch (B const & e) {
            try {
                std::rethrow_if_nested(e);
            } catch (A const & e) {
                try {
                    std::rethrow_if_nested(e);
                } catch (int const e) {
                    SHAREMIND_TESTASSERT(e == 42);
                } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
            } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
        } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
}
