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

#ifndef SHAREMIND_TEMPLATEAPPEND_H
#define SHAREMIND_TEMPLATEAPPEND_H


namespace sharemind {

template <typename T,
          typename TmplInstance,
          T ...>
struct TemplateAppend;

template <typename T,
          typename TmplInstance,
          T ... vs>
using TemplateAppend_t =
        typename TemplateAppend<T, TmplInstance, vs...>::type;

template <typename T,
          template <T ...> class Tmpl,
          T ... vs>
struct TemplateAppend<T, Tmpl<vs...> >
{ using type = Tmpl<vs...>; };

template <typename T,
          template <T ...> class Tmpl,
          T ... Tvs,
          T ... vs>
struct TemplateAppend<T, Tmpl<Tvs...>, vs...>
{ using type = Tmpl<Tvs..., vs...>; };

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATEAPPEND_H */
