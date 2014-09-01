/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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

#endif /* SHAREMIND_ENDIAN_MACROS_H */
