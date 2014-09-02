/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_COMMON_GCCINHERITCONSTRUCTOR_H
#define SHAREMIND_COMMON_GCCINHERITCONSTRUCTOR_H

#include "GccVersion.h"

/**
  We always expose this GCC 4.7 workaround macro for all compilers, because
  there are contexts where extracting a non-qualified (base) from a qualified
  name (fullBase) is impossible, and hence this is always required. Note that
  the third argument is not used.
*/
#define SHAREMIND_GCC_INHERITED_CONSTRUCTOR_WORKAROUND(thisClass,fullBase,base)\
    template <typename ... Args> \
    inline thisClass(Args && ... args) \
        : fullBase(std::forward<Args>(args)...) {}

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
#ifndef SHAREMIND_SILENCE_WORKAROUND_WARNINGS
#warning Using workaround for inherited constructors for g++ older than 4.8. \
         Define SHAREMIND_SILENCE_WORKAROUND_WARNINGS to silence.
#endif
#include <utility>
#define SHAREMIND_GCC_INHERITED_CONSTRUCTOR(...) \
        SHAREMIND_GCC_INHERITED_CONSTRUCTOR_WORKAROUND(__VA_ARGS__)
#else
#define SHAREMIND_GCC_INHERITED_CONSTRUCTOR(thisClass,fullBase,base) \
    using fullBase::base;
#endif

#endif /* SHAREMIND_COMMON_GCCINHERITCONSTRUCTOR_H */
