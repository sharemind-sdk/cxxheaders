/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMINDCOMMON_DEBUGVERBOSEHANDLERS_H
#define SHAREMINDCOMMON_DEBUGVERBOSEHANDLERS_H

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <typeinfo>
#include "compiler-support/GccNoreturn.h"


namespace {

void sharemindVerbosePrintException(std::exception_ptr e) noexcept {
    size_t n = 0u;
    do {
        try {
            std::rethrow_exception(e);
        } catch (const std::exception & se) {
            #ifdef __GLIBCXX__
            fprintf(stderr, "%zu: Exception of type %s, what(): %s\n",
                    ++n, e.__cxa_exception_type()->name(), se.what());
            #else
            fprintf(stderr, "%zu: Exception of unknown type derived from "
                    "std::exception, what(): %s\n", ++n, se.what());
            #endif
            const std::nested_exception * const ne =
                    dynamic_cast<const std::nested_exception *>(&se);
            if (!ne)
                break;
            e = ne->nested_ptr();
        } catch (...) {
            #ifdef __GLIBCXX__
            fprintf(stderr, "%zu: Exception of type %s.\n... unable to "
                    "continue printing nested exceptions, if any ...\n",
                    ++n, e.__cxa_exception_type()->name());
            #else
            fprintf(stderr, "%zu: Exception of unknown type not derived from "
                    "std::exception.\n... unable to continue printing nested "
                    "exceptions, if any ...\n", ++n);
            #endif
            return;
        }
    } while (e);
}

SHAREMIND_GCC_NORETURN_PART1
void sharemindVerboseTerminateHandler() noexcept
    SHAREMIND_GCC_NORETURN_PART2
{
    const std::exception_ptr e = std::current_exception();
    if (e) {
        fprintf(stderr, "std::terminate() called with active exception(s):\n");
        sharemindVerbosePrintException(e);
    } else {
        fprintf(stderr, "std::terminate() called!\n");
    }
    abort();
}


SHAREMIND_GCC_NORETURN_PART1
void sharemindVerboseUnexpectedHandler() noexcept
    SHAREMIND_GCC_NORETURN_PART2
{
    const std::exception_ptr e = std::current_exception();
    if (e) {
        fprintf(stderr,
                "std::unexpected() called with an active exception:\n");
        sharemindVerbosePrintException(e);
    } else {
        fprintf(stderr,
                "std::unexpected() called without an active exception!\n");
    }
    abort();
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
const std::terminate_handler sharemindVerboseTerminateHandler_old =
        std::set_terminate(sharemindVerboseTerminateHandler);
const std::unexpected_handler sharemindVerboseUnexpectedHandler_old =
        std::set_unexpected(sharemindVerboseUnexpectedHandler);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

} /* anonymous namespace */

#endif /* SHAREMINDCOMMON_DEBUGVERBOSEHANDLERS_H */
