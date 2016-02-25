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


namespace sharemind {

template <typename T,
          template <T> class Pred,
          typename TmplInstance,
          T ...>
struct TemplateCondAppend;

template <typename T,
          template <T> class Pred,
          typename TmplInstance,
          T ... vs>
using TemplateCondAppend_t =
        typename TemplateCondAppend<T, Pred, TmplInstance, vs...>::type;

template <typename T,
          template <T> class Pred,
          typename TmplInstance>
struct TemplateCondAppend<T, Pred, TmplInstance>
{ using type = TmplInstance; };

template <typename T,
          template <T> class Pred,
          template <T ...> class Tmpl,
          T ... Tvs,
          T v,
          T ... vs>
struct TemplateCondAppend<T, Pred, Tmpl<Tvs...>, v, vs...> {
    using type =
            typename ::std::conditional<
                Pred<v>::value,
                TemplateCondAppend_t<T, Pred, Tmpl<Tvs..., v>, vs...>,
                TemplateCondAppend_t<T, Pred, Tmpl<Tvs...>, vs...>
            >::type;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATECONDAPPEND_H */
