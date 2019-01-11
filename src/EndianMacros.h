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

#ifndef SHAREMIND_ENDIAN_MACROS_H
#define SHAREMIND_ENDIAN_MACROS_H

#include <cstdint>
#include <cstring>
#include <endian.h>
#include <sharemind/endian.h>
#include "StrongType.h"


#if (__BYTE_ORDER != __BIG_ENDIAN) && (__BYTE_ORDER != __LITTLE_ENDIAN)
#error Unsupported plaform!
#endif

namespace sharemind {

#define SHAREMIND_ENDIAN_ID_FUN(name,type) \
    inline type name(type const v) noexcept { return v; }
#define SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES(name) \
    SHAREMIND_ENDIAN_ID_FUN(name, int8_t) \
    SHAREMIND_ENDIAN_ID_FUN(name, int16_t) \
    SHAREMIND_ENDIAN_ID_FUN(name, int32_t) \
    SHAREMIND_ENDIAN_ID_FUN(name, int64_t) \
    SHAREMIND_ENDIAN_ID_FUN(name, uint8_t) \
    SHAREMIND_ENDIAN_ID_FUN(name, uint16_t) \
    SHAREMIND_ENDIAN_ID_FUN(name, uint32_t) \
    SHAREMIND_ENDIAN_ID_FUN(name, uint64_t)
#define SHAREMIND_ENDIAN_INT_FUN(name,signedType,fun) \
    inline signedType name(signedType v) noexcept { \
        u ## signedType c; \
        static_assert(sizeof(c) == sizeof(v), ""); \
        std::memcpy(&c, &v, sizeof(c)); \
        c = fun(c); \
        std::memcpy(&v, &c, sizeof(c)); \
        return v; \
    }

#if __BYTE_ORDER == __BIG_ENDIAN
SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES(hostToBigEndian)
SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES(bigEndianToHost)
#else
inline uint8_t  hostToBigEndian(uint8_t  const v) noexcept { return v; }
inline uint16_t hostToBigEndian(uint16_t const v) noexcept { return htobe16(v); }
inline uint32_t hostToBigEndian(uint32_t const v) noexcept { return htobe32(v); }
inline uint64_t hostToBigEndian(uint64_t const v) noexcept { return htobe64(v); }
inline int8_t  hostToBigEndian(int8_t  const v) noexcept { return v; }
SHAREMIND_ENDIAN_INT_FUN(hostToBigEndian, int16_t, htobe16)
SHAREMIND_ENDIAN_INT_FUN(hostToBigEndian, int32_t, htobe32)
SHAREMIND_ENDIAN_INT_FUN(hostToBigEndian, int64_t, htobe64)

inline uint8_t  bigEndianToHost(uint8_t  const v) noexcept { return v; }
inline uint16_t bigEndianToHost(uint16_t const v) noexcept { return be16toh(v); }
inline uint32_t bigEndianToHost(uint32_t const v) noexcept { return be32toh(v); }
inline uint64_t bigEndianToHost(uint64_t const v) noexcept { return be64toh(v); }
inline int8_t  bigEndianToHost(int8_t  const v) noexcept { return v; }
SHAREMIND_ENDIAN_INT_FUN(bigEndianToHost, int16_t, be16toh)
SHAREMIND_ENDIAN_INT_FUN(bigEndianToHost, int32_t, be32toh)
SHAREMIND_ENDIAN_INT_FUN(bigEndianToHost, int64_t, be64toh)
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES(hostToLittleEndian)
SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES(hostToSharemindNetOrder)
SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES(littleEndianToHost)
SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES(sharemindNetToHostOrder)
#else
inline uint8_t  hostToLittleEndian(uint8_t  const v) noexcept { return v; }
inline uint16_t hostToLittleEndian(uint16_t const v) noexcept { return htole16(v); }
inline uint32_t hostToLittleEndian(uint32_t const v) noexcept { return htole32(v); }
inline uint64_t hostToLittleEndian(uint64_t const v) noexcept { return htole64(v); }
inline int8_t  hostToLittleEndian(int8_t  const v) noexcept { return v; }
SHAREMIND_ENDIAN_INT_FUN(hostToLittleEndian, int16_t, htole16)
SHAREMIND_ENDIAN_INT_FUN(hostToLittleEndian, int32_t, htole32)
SHAREMIND_ENDIAN_INT_FUN(hostToLittleEndian, int64_t, htole64)
inline uint8_t  hostToSharemindNetOrder(uint8_t  const v) noexcept { return v; }
inline uint16_t hostToSharemindNetOrder(uint16_t const v) noexcept { return htole16(v); }
inline uint32_t hostToSharemindNetOrder(uint32_t const v) noexcept { return htole32(v); }
inline uint64_t hostToSharemindNetOrder(uint64_t const v) noexcept { return htole64(v); }
inline int8_t  hostToSharemindNetOrder(int8_t  const v) noexcept { return v; }
SHAREMIND_ENDIAN_INT_FUN(hostToSharemindNetOrder, int16_t, htole16)
SHAREMIND_ENDIAN_INT_FUN(hostToSharemindNetOrder, int32_t, htole32)
SHAREMIND_ENDIAN_INT_FUN(hostToSharemindNetOrder, int64_t, htole64)

inline uint8_t  littleEndianToHost(uint8_t  const v) noexcept { return v; }
inline uint16_t littleEndianToHost(uint16_t const v) noexcept { return le16toh(v); }
inline uint32_t littleEndianToHost(uint32_t const v) noexcept { return le32toh(v); }
inline uint64_t littleEndianToHost(uint64_t const v) noexcept { return le64toh(v); }
inline int8_t  littleEndianToHost(int8_t  const v) noexcept { return v; }
SHAREMIND_ENDIAN_INT_FUN(littleEndianToHost, int16_t, le16toh)
SHAREMIND_ENDIAN_INT_FUN(littleEndianToHost, int32_t, le32toh)
SHAREMIND_ENDIAN_INT_FUN(littleEndianToHost, int64_t, le64toh)
inline uint8_t  sharemindNetToHostOrder(uint8_t  const v) noexcept { return v; }
inline uint16_t sharemindNetToHostOrder(uint16_t const v) noexcept { return le16toh(v); }
inline uint32_t sharemindNetToHostOrder(uint32_t const v) noexcept { return le32toh(v); }
inline uint64_t sharemindNetToHostOrder(uint64_t const v) noexcept { return le64toh(v); }
inline int8_t  sharemindNetToHostOrder(int8_t  const v) noexcept { return v; }
SHAREMIND_ENDIAN_INT_FUN(sharemindNetToHostOrder, int16_t, le16toh)
SHAREMIND_ENDIAN_INT_FUN(sharemindNetToHostOrder, int32_t, le32toh)
SHAREMIND_ENDIAN_INT_FUN(sharemindNetToHostOrder, int64_t, le64toh)
#endif

#undef SHAREMIND_ENDIAN_ID_FUNS_ALLTYPES
#undef SHAREMIND_ENDIAN_ID_FUN
#undef SHAREMIND_ENDIAN_INT_FUN

#define SHAREMIND_ENDIAN_ST_FUN(type, fun) \
    template <typename Tag, typename ... Mixins> \
    StrongType<type, Tag, Mixins...> fun(StrongType<type, Tag, Mixins...> v) \
            noexcept \
    { return StrongType<type, Tag, Mixins...>(fun(v.get())); }
#define SHAREMIND_ENDIAN_ST_FUNS(type) \
    SHAREMIND_ENDIAN_ST_FUN(type, hostToBigEndian) \
    SHAREMIND_ENDIAN_ST_FUN(type, bigEndianToHost) \
    SHAREMIND_ENDIAN_ST_FUN(type, hostToLittleEndian) \
    SHAREMIND_ENDIAN_ST_FUN(type, littleEndianToHost) \
    SHAREMIND_ENDIAN_ST_FUN(type, hostToSharemindNetOrder) \
    SHAREMIND_ENDIAN_ST_FUN(type, sharemindNetToHostOrder)
SHAREMIND_ENDIAN_ST_FUNS(std::int8_t)
SHAREMIND_ENDIAN_ST_FUNS(std::int16_t)
SHAREMIND_ENDIAN_ST_FUNS(std::int32_t)
SHAREMIND_ENDIAN_ST_FUNS(std::int64_t)
SHAREMIND_ENDIAN_ST_FUNS(std::uint8_t)
SHAREMIND_ENDIAN_ST_FUNS(std::uint16_t)
SHAREMIND_ENDIAN_ST_FUNS(std::uint32_t)
SHAREMIND_ENDIAN_ST_FUNS(std::uint64_t)
#undef SHAREMIND_ENDIAN_ST_FUNS
#undef SHAREMIND_ENDIAN_ST_FUN

} /* namespace sharemind { */

#endif /* SHAREMIND_ENDIAN_MACROS_H */
