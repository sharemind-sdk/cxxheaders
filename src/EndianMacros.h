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

template <typename T> T hostToBigEndian(T);
template <> inline uint16_t hostToBigEndian(const uint16_t v) noexcept { return htobe16(v); }
template <> inline uint32_t hostToBigEndian(const uint32_t v) noexcept { return htobe32(v); }
template <> inline uint64_t hostToBigEndian(const uint64_t v) noexcept { return htobe64(v); }
template <> inline int16_t hostToBigEndian(const int16_t v) noexcept { return htobe16(v); }
template <> inline int32_t hostToBigEndian(const int32_t v) noexcept { return htobe32(v); }
template <> inline int64_t hostToBigEndian(const int64_t v) noexcept { return htobe64(v); }
template <typename T> T bigEndianToHost(T);
template <> inline uint16_t bigEndianToHost(const uint16_t v) noexcept { return be16toh(v); }
template <> inline uint32_t bigEndianToHost(const uint32_t v) noexcept { return be32toh(v); }
template <> inline uint64_t bigEndianToHost(const uint64_t v) noexcept { return be64toh(v); }
template <> inline int16_t bigEndianToHost(const int16_t v) noexcept { return be16toh(v); }
template <> inline int32_t bigEndianToHost(const int32_t v) noexcept { return be32toh(v); }
template <> inline int64_t bigEndianToHost(const int64_t v) noexcept { return be64toh(v); }
template <typename T> T hostToLittleEndian(T);
template <> inline uint16_t hostToLittleEndian(const uint16_t v) noexcept { return htole16(v); }
template <> inline uint32_t hostToLittleEndian(const uint32_t v) noexcept { return htole32(v); }
template <> inline uint64_t hostToLittleEndian(const uint64_t v) noexcept { return htole64(v); }
template <> inline int16_t hostToLittleEndian(const int16_t v) noexcept { return htole16(v); }
template <> inline int32_t hostToLittleEndian(const int32_t v) noexcept { return htole32(v); }
template <> inline int64_t hostToLittleEndian(const int64_t v) noexcept { return htole64(v); }
template <typename T> T littleEndianToHost(T);
template <> inline uint16_t littleEndianToHost(const uint16_t v) noexcept { return le16toh(v); }
template <> inline uint32_t littleEndianToHost(const uint32_t v) noexcept { return le32toh(v); }
template <> inline uint64_t littleEndianToHost(const uint64_t v) noexcept { return le64toh(v); }
template <> inline int16_t littleEndianToHost(const int16_t v) noexcept { return le16toh(v); }
template <> inline int32_t littleEndianToHost(const int32_t v) noexcept { return le32toh(v); }
template <> inline int64_t littleEndianToHost(const int64_t v) noexcept { return le64toh(v); }

} /* namespace sharemind { */

#endif /* SHAREMIND_ENDIAN_MACROS_H */
