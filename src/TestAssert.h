/*
 * Copyright (C) 2016 Cybernetica
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

#include <cstdio>
#include <cstdlib>
#include <exception>


#define SHAREMIND_TEST_PRINT_CURRENT_EXCEPTION \
    do { \
        std::exception_ptr eptr = std::current_exception(); \
        if (!eptr) { \
            std::fprintf(stderr, "<NO EXCEPTION>\n"); \
        } else { \
            /* Count exceptions: */ \
            std::size_t count = 0u; \
            { \
                std::exception_ptr eptr2 = eptr; \
                for (;;) { \
                    try { \
                        std::rethrow_exception(eptr2); \
                    } catch (std::exception const & e) { \
                        ++count; \
                        try { \
                            std::rethrow_if_nested(e); \
                            break; \
                        } catch (...) { \
                            eptr2 = std::current_exception(); \
                        } \
                    } catch (...) { \
                        ++count; \
                        break; \
                    } \
                } \
            } \
            /* Print exceptions: */ \
            std::size_t num = 1u; \
            for (;;) { \
                try { \
                    std::rethrow_exception(eptr); \
                } catch (std::exception const & e) { \
                    std::fprintf(stderr, \
                                 "[%zu/%zu] %s\n", \
                                 num, \
                                 count, \
                                 e.what()); \
                    std::fflush(stderr); \
                    ++num; \
                    try { \
                        std::rethrow_if_nested(e); \
                        break; \
                    } catch (...) { \
                        eptr = std::current_exception(); \
                    } \
                } catch (...) { \
                    std::fprintf( \
                            stderr, \
                            "[%zu/%zu] <UNKNOWN NON-STANDARD EXCEPTION>\n", \
                            num, \
                            count); \
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
