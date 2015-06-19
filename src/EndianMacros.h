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
inline int8_t  hostToBigEndian(int8_t  const v) noexcept { return v; }
inline int16_t hostToBigEndian(int16_t const v) noexcept { return htobe16(v); }
inline int32_t hostToBigEndian(int32_t const v) noexcept { return htobe32(v); }
inline int64_t hostToBigEndian(int64_t const v) noexcept { return htobe64(v); }

inline uint8_t  bigEndianToHost(uint8_t  const v) noexcept { return v; }
inline uint16_t bigEndianToHost(uint16_t const v) noexcept { return be16toh(v); }
inline uint32_t bigEndianToHost(uint32_t const v) noexcept { return be32toh(v); }
inline uint64_t bigEndianToHost(uint64_t const v) noexcept { return be64toh(v); }
inline int8_t  bigEndianToHost(int8_t  const v) noexcept { return v; }
inline int16_t bigEndianToHost(int16_t const v) noexcept { return be16toh(v); }
inline int32_t bigEndianToHost(int32_t const v) noexcept { return be32toh(v); }
inline int64_t bigEndianToHost(int64_t const v) noexcept { return be64toh(v); }

inline uint8_t  hostToLittleEndian(uint8_t  const v) noexcept { return v; }
inline uint16_t hostToLittleEndian(uint16_t const v) noexcept { return htole16(v); }
inline uint32_t hostToLittleEndian(uint32_t const v) noexcept { return htole32(v); }
inline uint64_t hostToLittleEndian(uint64_t const v) noexcept { return htole64(v); }
inline int8_t  hostToLittleEndian(int8_t  const v) noexcept { return v; }
inline int16_t hostToLittleEndian(int16_t const v) noexcept { return htole16(v); }
inline int32_t hostToLittleEndian(int32_t const v) noexcept { return htole32(v); }
inline int64_t hostToLittleEndian(int64_t const v) noexcept { return htole64(v); }
inline uint8_t  hostToNetOrder(uint8_t  const v) noexcept { return v; }
inline uint16_t hostToNetOrder(uint16_t const v) noexcept { return htole16(v); }
inline uint32_t hostToNetOrder(uint32_t const v) noexcept { return htole32(v); }
inline uint64_t hostToNetOrder(uint64_t const v) noexcept { return htole64(v); }
inline int8_t  hostToNetOrder(int8_t  const v) noexcept { return v; }
inline int16_t hostToNetOrder(int16_t const v) noexcept { return htole16(v); }
inline int32_t hostToNetOrder(int32_t const v) noexcept { return htole32(v); }
inline int64_t hostToNetOrder(int64_t const v) noexcept { return htole64(v); }

inline uint8_t  littleEndianToHost(uint8_t  const v) noexcept { return v; }
inline uint16_t littleEndianToHost(uint16_t const v) noexcept { return le16toh(v); }
inline uint32_t littleEndianToHost(uint32_t const v) noexcept { return le32toh(v); }
inline uint64_t littleEndianToHost(uint64_t const v) noexcept { return le64toh(v); }
inline int8_t  littleEndianToHost(int8_t  const v) noexcept { return v; }
inline int16_t littleEndianToHost(int16_t const v) noexcept { return le16toh(v); }
inline int32_t littleEndianToHost(int32_t const v) noexcept { return le32toh(v); }
inline int64_t littleEndianToHost(int64_t const v) noexcept { return le64toh(v); }
inline uint8_t  netToHostOrder(uint8_t  const v) noexcept { return v; }
inline uint16_t netToHostOrder(uint16_t const v) noexcept { return le16toh(v); }
inline uint32_t netToHostOrder(uint32_t const v) noexcept { return le32toh(v); }
inline uint64_t netToHostOrder(uint64_t const v) noexcept { return le64toh(v); }
inline int8_t  netToHostOrder(int8_t  const v) noexcept { return v; }
inline int16_t netToHostOrder(int16_t const v) noexcept { return le16toh(v); }
inline int32_t netToHostOrder(int32_t const v) noexcept { return le32toh(v); }
inline int64_t netToHostOrder(int64_t const v) noexcept { return le64toh(v); }

} /* namespace sharemind { */

#endif /* SHAREMIND_ENDIAN_MACROS_H */
