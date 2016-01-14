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

#ifndef SHAREMIND_COPYCV_H
#define SHAREMIND_COPYCV_H

#include "CopyConst.h"
#include "CopyVolatile.h"


namespace sharemind {

template <typename To, typename From>
using CopyCv = CopyConst<typename CopyVolatile<To, From>::type, From>;

template <typename To, typename From>
using CopyCv_t = typename CopyCv<To, From>::type;

template <typename To, typename From>
using OrCv = OrConst<OrVolatile<To, From>, From>;

template <typename To, typename From>
using OrCv_t = typename OrCv<To, From>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_COPYCV_H */
