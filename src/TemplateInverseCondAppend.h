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

#ifndef SHAREMIND_TEMPLATEINVERSECONDAPPEND_H
#define SHAREMIND_TEMPLATEINVERSECONDAPPEND_H

#include <type_traits>
#include "TemplateAppendOne.h"


namespace sharemind {

template <typename T,
          template <T> class Pred,
          template <T ...> class Tmpl,
          typename TmplInstance,
          T ...>
struct TemplateInverseCondAppend;

template <typename T,
          template <T> class Pred,
          template <T ...> class Tmpl,
          typename TmplInstance,
          T ... vs>
using TemplateInverseCondAppend_t =
        typename TemplateInverseCondAppend<T, Pred, Tmpl, TmplInstance, vs...>::type;

template <typename T,
          template <T> class Pred,
          template <T ...> class Tmpl,
          typename TmplInstance>
struct TemplateInverseCondAppend<T, Pred, Tmpl, TmplInstance>
{ using type = TmplInstance; };

template <typename T,
          template <T> class Pred,
          template <T ...> class Tmpl,
          typename TmplInstance,
          T v,
          T ... vs>
struct TemplateInverseCondAppend<T, Pred, Tmpl, TmplInstance, v, vs...> {
    using type =
            typename ::std::conditional<
                Pred<v>::value,
                TemplateInverseCondAppend_t<T, Pred, Tmpl, TmplInstance, vs...>,
                TemplateInverseCondAppend<
                    T,
                    Pred,
                    Tmpl,
                    TemplateAppendOne<T, Tmpl, TmplInstance, v>,
                    vs...
                >
            >::type;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATEINVERSECONDAPPEND_H */
