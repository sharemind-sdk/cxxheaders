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

#ifndef SHAREMIND_DEBUGVERBOSEHANDLERS_H
#define SHAREMIND_DEBUGVERBOSEHANDLERS_H

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <typeinfo>
#include "compiler-support/GccNoreturn.h"


namespace sharemind {

inline void verbosePrintException(std::exception_ptr e,
                                  std::ostream & oss = std::cerr) noexcept
{
    #ifdef __GLIBCXX__
    #define SHAREMIND_T << e.__cxa_exception_type()->name() <<
    #else
    #define SHAREMIND_T "<unknown type>"
    #endif
    assert(e);
    for (size_t n = 1u;; n++) {
        try {
            std::rethrow_exception(e);
        } catch (const std::exception & se) {
            oss << ++n << ": [" SHAREMIND_T "] " << se.what() << std::endl;
            std::nested_exception const * const ne =
                    dynamic_cast<std::nested_exception const *>(&se);
            if (!ne)
                return;
            e = ne->nested_ptr();
            assert(e);
        } catch (...) {
            oss << ++n << ": [" SHAREMIND_T "]" << std::endl
                << "... unable to continue printing nested exceptions, if any "
                   "..." << std::endl;
            return;
        }
    }
    #undef SHAREMIND_T
}

SHAREMIND_GCC_NORETURN_PART1
void verboseTerminateHandler() noexcept SHAREMIND_GCC_NORETURN_PART2;

void verboseTerminateHandler() noexcept {
    if (std::exception_ptr const e = std::current_exception()) {
        verbosePrintException(e,
                              std::cerr << "std::terminate() called with "
                                           "active exception(s):" << std::endl);
    } else {
        std::cerr << "std::terminate() called without active exceptions!"
                  << std::endl;
    }
    abort();
}


SHAREMIND_GCC_NORETURN_PART1
void verboseUnexpectedHandler() noexcept SHAREMIND_GCC_NORETURN_PART2;

void verboseUnexpectedHandler() noexcept {
    if (std::exception_ptr const e = std::current_exception()) {
        verbosePrintException(e,
                              std::cerr << "std::unexpected() called with "
                                           "active exception(s):" << std::endl);
    } else {
        std::cerr << "std::unexpected() called without active exceptions!"
                  << std::endl;
    }
    abort();
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
std::terminate_handler const verboseTerminateHandler_old =
        std::set_terminate(verboseTerminateHandler);
std::unexpected_handler const verboseUnexpectedHandler_old =
        std::set_unexpected(verboseUnexpectedHandler);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

} /* namespace sharemind { */

#endif /* SHAREMIND_DEBUGVERBOSEHANDLERS_H */
