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
