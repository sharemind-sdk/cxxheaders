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

#ifdef __MACH__

#include <libkern/OSByteOrder.h>
#define htobe16(x) (OSSwapHostToBigInt16((x)))
#define htole16(x) (OSSwapHostToLittleInt16((x)))
#define be16toh(x) (OSSwapBigToHostInt16((x)))
#define le16toh(x) (OSSwapLittleToHostInt16((x)))
#define htobe32(x) (OSSwapHostToBigInt32((x)))
#define htole32(x) (OSSwapHostToLittleInt32((x)))
#define be32toh(x) (OSSwapBigToHostInt32((x)))
#define le32toh(x) (OSSwapLittleToHostInt32((x)))
#define htobe64(x) (OSSwapHostToBigInt64((x)))
#define htole64(x) (OSSwapHostToLittleInt64((x)))
#define be64toh(x) (OSSwapBigToHostInt64((x)))
#define le64toh(x) (OSSwapLittleToHostInt64((x)))

#elif __FreeBSD__

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

#include <sys/endian.h>

#else

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <endian.h>

#endif

#ifdef __cplusplus

#include <cstdint>


namespace sharemind {

template <typename T> T hostToBigEndian(T);
template <> inline uint16_t hostToBigEndian(const uint16_t v) { return htobe16(v); }
template <> inline uint32_t hostToBigEndian(const uint32_t v) { return htobe32(v); }
template <> inline uint64_t hostToBigEndian(const uint64_t v) { return htobe64(v); }
template <> inline int16_t hostToBigEndian(const int16_t v) { return htobe16(v); }
template <> inline int32_t hostToBigEndian(const int32_t v) { return htobe32(v); }
template <> inline int64_t hostToBigEndian(const int64_t v) { return htobe64(v); }
template <typename T> T bigEndianToHost(T);
template <> inline uint16_t bigEndianToHost(const uint16_t v) { return be16toh(v); }
template <> inline uint32_t bigEndianToHost(const uint32_t v) { return be32toh(v); }
template <> inline uint64_t bigEndianToHost(const uint64_t v) { return be64toh(v); }
template <> inline int16_t bigEndianToHost(const int16_t v) { return be16toh(v); }
template <> inline int32_t bigEndianToHost(const int32_t v) { return be32toh(v); }
template <> inline int64_t bigEndianToHost(const int64_t v) { return be64toh(v); }
template <typename T> T hostToLittleEndian(T);
template <> inline uint16_t hostToLittleEndian(const uint16_t v) { return htole16(v); }
template <> inline uint32_t hostToLittleEndian(const uint32_t v) { return htole32(v); }
template <> inline uint64_t hostToLittleEndian(const uint64_t v) { return htole64(v); }
template <> inline int16_t hostToLittleEndian(const int16_t v) { return htole16(v); }
template <> inline int32_t hostToLittleEndian(const int32_t v) { return htole32(v); }
template <> inline int64_t hostToLittleEndian(const int64_t v) { return htole64(v); }
template <typename T> T littleEndianToHost(T);
template <> inline uint16_t littleEndianToHost(const uint16_t v) { return le16toh(v); }
template <> inline uint32_t littleEndianToHost(const uint32_t v) { return le32toh(v); }
template <> inline uint64_t littleEndianToHost(const uint64_t v) { return le64toh(v); }
template <> inline int16_t littleEndianToHost(const int16_t v) { return le16toh(v); }
template <> inline int32_t littleEndianToHost(const int32_t v) { return le32toh(v); }
template <> inline int64_t littleEndianToHost(const int64_t v) { return le64toh(v); }

} /* namespace sharemind { */

#endif

#endif /* SHAREMIND_ENDIAN_MACROS_H */
