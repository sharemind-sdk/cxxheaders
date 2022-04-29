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

#ifndef SHAREMIND_THROWNESTED_H
#define SHAREMIND_THROWNESTED_H

#include <utility>
#include "ThrowWithNested.h"


namespace sharemind {

template <typename Exception>
[[noreturn]] inline void throwNested(Exception && exception)
{ throw std::forward<Exception>(exception); }

template <typename Exception, typename Exception2, typename ... Exceptions>
[[noreturn]] inline void throwNested(Exception && exception,
                                     Exception2 && exception2,
                                     Exceptions && ... exceptions)
{
    try {
        throw std::forward<Exception>(exception);
    } catch (...) {
        throwWithNested(std::forward<Exception2>(exception2),
                        std::forward<Exceptions>(exceptions)...);
    }
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_THROWNESTED_H */
