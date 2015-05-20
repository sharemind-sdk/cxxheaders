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

#ifndef SHAREMIND_GCCINHERITCONSTRUCTOR_H
#define SHAREMIND_GCCINHERITCONSTRUCTOR_H

#include <sharemind/compiler-support/GccVersion.h>


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
#include <utility>
#define SHAREMIND_GCC_INHERITED_CONSTRUCTOR(...) \
        SHAREMIND_GCC_INHERITED_CONSTRUCTOR_WORKAROUND(__VA_ARGS__)
#else
#define SHAREMIND_GCC_INHERITED_CONSTRUCTOR(thisClass,fullBase,base) \
    using fullBase::base;
#endif

#endif /* SHAREMIND_GCCINHERITCONSTRUCTOR_H */