/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_TEMPLATEAPPEND_H
#define SHAREMIND_TEMPLATEAPPEND_H

#include "AlwaysTrueCond.h"
#include "TemplateCondAppend.h"


namespace sharemind {

template <template <typename ...> class Tmpl,
          typename Tpl,
          typename ... Ts>
using TemplateAppend = TemplateCondAppend<AlwaysTrueCond, Tmpl, Tpl, Ts...>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATEAPPEND_H */
