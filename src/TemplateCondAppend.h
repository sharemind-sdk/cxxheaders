/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_TEMPLATECONDAPPEND_H
#define SHAREMIND_TEMPLATECONDAPPEND_H

#include "TemplateAppendOne.h"


namespace sharemind {

template <template <typename T> class Cond,
          template <typename ...> class Tmpl,
          typename Tpl,
          typename ...>
struct TemplateCondAppend;

template <template <typename T> class Cond,
          template <typename ...> class Tmpl,
          typename Tpl>
struct TemplateCondAppend<Cond, Tmpl, Tpl>
{ using type = Tpl; };

template <template <typename T> class Cond,
          template <typename ...> class Tmpl,
          typename Tpl,
          typename T,
          typename ... Ts>
struct TemplateCondAppend<Cond, Tmpl, Tpl, T, Ts...> {
  using type =
      typename ::std::conditional<
          Cond<T>::value,
          typename TemplateCondAppend<Cond, Tmpl, Tpl, Ts...>::type,
          typename TemplateCondAppend<
                  Cond,
                  Tmpl,
                  typename TemplateAppendOne<Tmpl, Tpl, T>::type,
                  Ts...>::type
      >::type;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATECONDAPPEND_H */
