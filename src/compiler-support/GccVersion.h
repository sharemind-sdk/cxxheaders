/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_GCCVERSION_H
#define SHAREMIND_GCCVERSION_H


#if !defined(__clang__) && defined(__GNUC__)
#define SHAREMIND_GCC_VERSION \
    (((__GNUC__ * 100) + __GNUC_MINOR__) * 100 + __GNUC_PATCHLEVEL__)
#endif

#endif /* SHAREMIND_GCCVERSION_H */
