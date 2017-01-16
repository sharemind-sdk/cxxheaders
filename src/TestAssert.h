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


#define SHAREMIND_TEST_UNREACHABLE \
    do { \
        std::fprintf(stderr,  __FILE__ ":%d: %s: Sharemind test unreachable code reached!\n", __LINE__, __PRETTY_FUNCTION__); \
        std::fflush(stderr); \
        std::abort(); \
    } while(false)

#define SHAREMIND_TESTASSERT(...) \
    do { \
        if (!(__VA_ARGS__)) { \
            std::fprintf(stderr,  __FILE__ ":%d: %s: Sharemind test assertion `" #__VA_ARGS__ "' failed!\n", __LINE__, __PRETTY_FUNCTION__); \
            std::fflush(stderr); \
            std::abort(); \
        } \
    } while(false)

#endif /* SHAREMIND_TESTASSERT_H */
