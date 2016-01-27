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

#ifndef SHAREMIND_MAYBEDYNAMICPACKEDSTRUCT_H
#define SHAREMIND_MAYBEDYNAMICPACKEDSTRUCT_H

#include <type_traits>
#include "DynamicPackedStruct.h"
#include "DynamicPackingInfo.h"
#include "PackedStruct.h"


namespace sharemind {

template <typename ... Ts>
using MaybeDynamicPackedStruct =
        typename std::conditional<
            DynamicPackingInfo<Ts...>::hasDynamicTypes,
            DynamicPackedStruct<Ts...>,
            PackedStruct<Ts...>
        >::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_MAYBEDYNAMICPACKEDSTRUCT_H */
