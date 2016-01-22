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

#ifndef SHAREMIND_PACKEDRAWDATA_H
#define SHAREMIND_PACKEDRAWDATA_H

#include <cstddef>
#include <cstdint>

namespace sharemind {

template <std::size_t size>
union __attribute__((packed)) PackedRawData {
    uint8_t asUint8[size];
    char asChar[size];
    unsigned char asUChar[size];
} __attribute__((packed));
static_assert(sizeof(PackedRawData<1u>) == 1u, "");
static_assert(sizeof(PackedRawData<2u>) == 2u, "");

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKEDRAWDATA_H */
