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

#include "../src/ThrowWithNested.h"

#include "../src/TestAssert.h"


using sharemind::throwWithNested;

using FirstException = int;
struct A {};
struct B {};
struct C {};

int main() {
    try {
        try {
            throw FirstException(42);
        } catch (...) {
            throwWithNested();
        }
    } catch (FirstException const & e) {
        SHAREMIND_TESTASSERT(e == 42);
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }

    try {
        try {
            throw FirstException(42);
        } catch (...) {
            throwWithNested(A());
        }
    } catch (A const & e) {
        try {
            std::rethrow_if_nested(e);
        } catch (FirstException const & e) {
            SHAREMIND_TESTASSERT(e == 42);
        } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }

    try {
        try {
            throw FirstException(42);
        } catch (...) {
            throwWithNested(A(), B());
        }
    } catch (B const & e) {
        try {
            std::rethrow_if_nested(e);
        } catch (A const & e) {
            try {
                std::rethrow_if_nested(e);
            } catch (FirstException const & e) {
                SHAREMIND_TESTASSERT(e == 42);
            } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
        } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }

    try {
        try {
            throw FirstException(42);
        } catch (...) {
            throwWithNested(A(), B(), C());
        }
    } catch (C const & e) {
        try {
            std::rethrow_if_nested(e);
        } catch (B const & e) {
            try {
                std::rethrow_if_nested(e);
            } catch (A const & e) {
                try {
                    std::rethrow_if_nested(e);
                } catch (FirstException const & e) {
                    SHAREMIND_TESTASSERT(e == 42);
                } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
            } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
        } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
    } catch (...) { SHAREMIND_TEST_UNREACHABLE; }
}
