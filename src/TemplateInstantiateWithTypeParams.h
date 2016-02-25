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

#ifndef SHAREMIND_TEMPLATEINSTANTIATEWITHTYPEPARAMS_H
#define SHAREMIND_TEMPLATEINSTANTIATEWITHTYPEPARAMS_H

#include "TemplateTypeList.h"


namespace sharemind {

template <template <typename ...> class Tmpl, typename ... From>
struct TemplateInstantiateWithTypeParams;

template <template <typename ...> class Tmpl, typename ... From>
using TemplateInstantiateWithTypeParams_t =
        typename TemplateInstantiateWithTypeParams<Tmpl, From...>::type;

template <template <typename ...> class Tmpl>
struct TemplateInstantiateWithTypeParams<Tmpl>
{ using type = Tmpl<>; };

template <template <typename ...> class Tmpl,
          template <typename ...> class T,
          typename ... Ts>
struct TemplateInstantiateWithTypeParams<Tmpl, T<Ts...> >
{ using type = Tmpl<Ts...>; };

template <template <typename ...> class Tmpl,
          template <typename ...> class T,
          typename ... TTs,
          template <typename ...> class T2,
          typename ... T2Ts,
          typename ... From>
struct TemplateInstantiateWithTypeParams<
        Tmpl,
        T<TTs...>,
        T2<T2Ts...>,
        From...>
{
    using type =
            TemplateInstantiateWithTypeParams_t<
                Tmpl,
                TemplateTypeList<TTs..., T2Ts...>,
                From...>;
};

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATEINSTANTIATEWITHTYPEPARAMS_H */
