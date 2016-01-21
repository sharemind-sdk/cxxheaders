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

#include <type_traits>
#include "TemplateAppendOneType.h"


namespace sharemind {

template <template <typename T> class Pred,
          template <typename ...> class Tmpl,
          typename TmplInstance,
          typename ...>
struct TemplateCondAppendType;

template <template <typename T> class Pred,
          template <typename ...> class Tmpl,
          typename TmplInstance>
struct TemplateCondAppendType<Pred, Tmpl, TmplInstance>
{ using type = TmplInstance; };

template <template <typename T> class Pred,
          template <typename ...> class Tmpl,
          typename TmplInstance,
          typename T,
          typename ... Ts>
struct TemplateCondAppendType<Pred, Tmpl, TmplInstance, T, Ts...> {
  using type =
      typename ::std::conditional<
          !Pred<T>::value,
          typename TemplateCondAppendType<Pred, Tmpl, TmplInstance, Ts...>::type,
          typename TemplateCondAppendType<
                  Pred,
                  Tmpl,
                  typename TemplateAppendOneType<Tmpl, TmplInstance, T>::type,
                  Ts...>::type
      >::type;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATECONDAPPEND_H */
