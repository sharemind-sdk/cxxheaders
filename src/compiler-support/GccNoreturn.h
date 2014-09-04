/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_NORETURN_H
#define SHAREMIND_NORETURN_H

#include "GccVersion.h"


#if !defined(SHAREMIND_GCC_VERSION) || (SHAREMIND_GCC_VERSION >= 40800)
#define SHAREMIND_GCC_NORETURN_PART1 [[noreturn]]
#define SHAREMIND_GCC_NORETURN_PART2
#else
#define SHAREMIND_GCC_NORETURN_PART1
#define SHAREMIND_GCC_NORETURN_PART2 __attribute__ ((noreturn))
#endif

#endif /* SHAREMIND_NORETURN_H */
