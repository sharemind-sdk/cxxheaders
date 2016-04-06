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

#ifndef SHAREMIND_CSTDINTLITERALS_H
#define SHAREMIND_CSTDINTLITERALS_H

#include <cstddef>
#include "IntegerLiterals.h"


SHAREMIND_INTEGERLITERAL_NS_WRAP(
        SHAREMIND_INTEGERLITERAL_SIMPLE_TEMPLATE(SizetLiteral,std::size_t))

namespace sharemind {

SHAREMIND_INTEGERLITERAL_UNSIGNED_OPS(std::size_t,
                                      sizeT,
                                      SHAREMIND_INTEGERLITERAL_NS::SizetLiteral)

} /* namespace sharemind { */

#endif /* #ifndef SHAREMIND_CSTDINTLITERALS_H */
