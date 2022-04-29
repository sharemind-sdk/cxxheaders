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

#ifndef SHAREMIND_TESTASSERT_H
#define SHAREMIND_TESTASSERT_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>


#define SHAREMIND_TEST_PRINT_CURRENT_EXCEPTION \
    do { \
        auto SHAREMIND_TESTASSERT_eptr = std::current_exception(); \
        if (!SHAREMIND_TESTASSERT_eptr) { \
            std::fprintf(stderr, "<NO EXCEPTION>\n"); \
        } else { \
            /* Count exceptions: */ \
            std::size_t SHAREMIND_TESTASSERT_count = 0u; \
            { \
                auto SHAREMIND_TESTASSERT_eptr2 = SHAREMIND_TESTASSERT_eptr; \
                for (;;) { \
                    try { \
                        std::rethrow_exception(SHAREMIND_TESTASSERT_eptr2); \
                    } catch (std::exception const & SHAREMIND_TESTASSERT_e) { \
                        ++SHAREMIND_TESTASSERT_count; \
                        try { \
                            std::rethrow_if_nested(SHAREMIND_TESTASSERT_e); \
                            break; \
                        } catch (...) { \
                            SHAREMIND_TESTASSERT_eptr2 = \
                                    std::current_exception(); \
                        } \
                    } catch (...) { \
                        ++SHAREMIND_TESTASSERT_count; \
                        break; \
                    } \
                } \
            } \
            /* Print exceptions: */ \
            std::size_t num = 1u; \
            for (;;) { \
                try { \
                    std::rethrow_exception(SHAREMIND_TESTASSERT_eptr); \
                } catch (std::exception const & SHAREMIND_TESTASSERT_e) { \
                    std::fprintf(stderr, \
                                 "[%zu/%zu] %s\n", \
                                 num, \
                                 SHAREMIND_TESTASSERT_count, \
                                 SHAREMIND_TESTASSERT_e.what()); \
                    std::fflush(stderr); \
                    ++num; \
                    try { \
                        std::rethrow_if_nested(SHAREMIND_TESTASSERT_e); \
                        break; \
                    } catch (...) { \
                        SHAREMIND_TESTASSERT_eptr = std::current_exception(); \
                    } \
                } catch (...) { \
                    std::fprintf( \
                            stderr, \
                            "[%zu/%zu] <UNKNOWN NON-STANDARD EXCEPTION>\n", \
                            num, \
                            SHAREMIND_TESTASSERT_count); \
                    std::fflush(stderr); \
                    break; \
                } \
            } \
        } \
    } while(false)

#define SHAREMIND_TEST_UNREACHABLE \
    do { \
        std::fprintf( \
                stderr,  \
                __FILE__ ":%d: %s: Sharemind test unreachable code reached!\n",\
                __LINE__, \
                __PRETTY_FUNCTION__); \
        std::fflush(stderr); \
        std::abort(); \
    } while(false)

#define SHAREMIND_TEST_DOES_NOT_THROW(...) \
    do { \
        try { \
            __VA_ARGS__ ; \
        } catch (...) { \
            std::fprintf(\
                    stderr, \
                    __FILE__ ":%d: %s: Sharemind test expression does not " \
                    "throw assertion failed for  `" #__VA_ARGS__ "'!\n", \
                    __LINE__, \
                    __PRETTY_FUNCTION__); \
            std::fflush(stderr); \
            SHAREMIND_TEST_PRINT_CURRENT_EXCEPTION; \
            std::abort(); \
        } \
    } while(false)

#define SHAREMIND_TEST_THROWS(...) \
    do { \
        try { \
            { __VA_ARGS__ ; } \
            std::fprintf( \
                    stderr, \
                    __FILE__ ":%d: %s: Sharemind test expression throws " \
                    "assertion failed for `" #__VA_ARGS__ "'!\n", \
                    __LINE__, \
                    __PRETTY_FUNCTION__); \
            std::fflush(stderr); \
            std::abort(); \
        } catch (...) { } \
    } while(false)

#define SHAREMIND_TESTASSERT(...) \
    do { \
        try { \
            if (!(__VA_ARGS__)) { \
                std::fprintf( \
                        stderr, \
                        __FILE__ ":%d: %s: Sharemind test assertion `" \
                        #__VA_ARGS__ "' failed!\n", \
                        __LINE__, \
                        __PRETTY_FUNCTION__); \
                std::fflush(stderr); \
                std::abort(); \
            } \
        } catch(...) { \
            std::fprintf( \
                    stderr, \
                    __FILE__ ":%d: %s: Sharemind test assertion `" \
                    #__VA_ARGS__ "' failed due to unexpected exception!\n", \
                    __LINE__, \
                    __PRETTY_FUNCTION__); \
            std::fflush(stderr); \
            SHAREMIND_TEST_PRINT_CURRENT_EXCEPTION; \
            std::abort(); \
        } \
    } while(false)

#endif /* SHAREMIND_TESTASSERT_H */
