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

#ifndef SHAREMIND_ASSERTRETURN_H
#define SHAREMIND_ASSERTRETURN_H

#include <cassert>

#ifndef NDEBUG
#define SHAREMIND_ASSERTRETURN(...) \
    [](decltype(__VA_ARGS__) v) \
        noexcept(noexcept(__VA_ARGS__)) \
        -> decltype(__VA_ARGS__) \
    { \
        assert(v); \
        return v; \
    }(__VA_ARGS__)
#else
#define SHAREMIND_ASSERTRETURN(...) __VA_ARGS__
#endif


#endif /* SHAREMIND_ASSERTRETURN_H */
