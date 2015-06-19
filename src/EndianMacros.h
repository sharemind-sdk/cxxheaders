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

#ifndef SHAREMIND_ENDIAN_MACROS_H
#define SHAREMIND_ENDIAN_MACROS_H

#include <cstdint>
#include <sharemind/endian.h>


namespace sharemind {

inline uint8_t  hostToBigEndian(uint8_t  const v) noexcept { return v; }
inline uint16_t hostToBigEndian(uint16_t const v) noexcept { return htobe16(v); }
inline uint32_t hostToBigEndian(uint32_t const v) noexcept { return htobe32(v); }
inline uint64_t hostToBigEndian(uint64_t const v) noexcept { return htobe64(v); }

inline uint8_t  bigEndianToHost(uint8_t  const v) noexcept { return v; }
inline uint16_t bigEndianToHost(uint16_t const v) noexcept { return be16toh(v); }
inline uint32_t bigEndianToHost(uint32_t const v) noexcept { return be32toh(v); }
inline uint64_t bigEndianToHost(uint64_t const v) noexcept { return be64toh(v); }

inline uint8_t  hostToLittleEndian(uint8_t  const v) noexcept { return v; }
inline uint16_t hostToLittleEndian(uint16_t const v) noexcept { return htole16(v); }
inline uint32_t hostToLittleEndian(uint32_t const v) noexcept { return htole32(v); }
inline uint64_t hostToLittleEndian(uint64_t const v) noexcept { return htole64(v); }

inline uint8_t  littleEndianToHost(uint8_t  const v) noexcept { return v; }
inline uint16_t littleEndianToHost(uint16_t const v) noexcept { return le16toh(v); }
inline uint32_t littleEndianToHost(uint32_t const v) noexcept { return le32toh(v); }
inline uint64_t littleEndianToHost(uint64_t const v) noexcept { return le64toh(v); }

} /* namespace sharemind { */

#endif /* SHAREMIND_ENDIAN_MACROS_H */
