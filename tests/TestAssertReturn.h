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

#include "../src/AssertReturn.h"

#include <memory>

#ifndef NDEBUG
#include <cassert>
#include <cstdlib>
#include <signal.h>

[[noreturn]] void handleAbort(int) { exit(0); }
#define MYASSERT(...) assert(__VA_ARGS__);
#else
#define MYASSERT(...) if (!(__VA_ARGS__)) return EXIT_FAILURE;
#endif


int main() {
    auto sPtr = std::make_shared<int>(42);
    std::weak_ptr<int> wPtr(sPtr);
    auto sPtr2 = SHAREMIND_ASSERTRETURN(wPtr.lock());
    MYASSERT(sPtr2.use_count() == 2u);

    sPtr.reset();
    sPtr2.reset();

    #ifndef NDEBUG
    signal(SIGABRT, &handleAbort);
    #endif

    auto const sPtr3 = SHAREMIND_ASSERTRETURN(wPtr.lock());
    MYASSERT(!sPtr3);
    #ifndef NDEBUG
    return EXIT_FAILURE;
    #endif
}