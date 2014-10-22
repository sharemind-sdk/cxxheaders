/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMINDCOMMON_VOIDT_H
#define SHAREMINDCOMMON_VOIDT_H

#include "compiler-support/GccVersion.h"


namespace sharemind {

#ifdef SHAREMIND_GCC_VERSION
/* Work around CWG 1558 in GCC: */
namespace Detail {
    template <typename ...> struct Voider { using type = void; };
} /* namespace Detail { */
template <typename ... T> using VoidT = typename Detail::Voider<T ...>::type;
#else
template <typename ...> using VoidT = void;
#endif

} /* namespace sharemind { */

#endif /* SHAREMINDCOMMON_VOIDT_H */
