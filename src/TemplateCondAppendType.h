/*
 * Copyright (C) Cybernetica
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

#ifndef SHAREMIND_TEMPLATECONDAPPENDTYPE_H
#define SHAREMIND_TEMPLATECONDAPPENDTYPE_H

#include <type_traits>


namespace sharemind {

template <template <typename T> class Pred,
          typename TmplInstance,
          typename ...>
struct TemplateCondAppendType;

template <template <typename T> class Pred,
          typename TmplInstance,
          typename ... Ts>
using TemplateCondAppendType_t =
        typename TemplateCondAppendType<Pred, TmplInstance, Ts...>::type;

template <template <typename T> class Pred,
          typename TmplInstance>
struct TemplateCondAppendType<Pred, TmplInstance>
{ using type = TmplInstance; };

template <template <typename T> class Pred,
          template <typename ...> class Tmpl,
          typename ... TTs,
          typename T,
          typename ... Ts>
struct TemplateCondAppendType<Pred, Tmpl<TTs...>, T, Ts...> {
    using type =
            typename ::std::conditional<
                Pred<T>::value,
                TemplateCondAppendType_t<Pred, Tmpl<TTs..., T>, Ts... >,
                TemplateCondAppendType_t<Pred, Tmpl<TTs...>, Ts...>
            >::type;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATECONDAPPENDTYPE_H */
