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

#ifndef SHAREMIND_TEMPLATEAPPENDTYPES_H
#define SHAREMIND_TEMPLATEAPPENDTYPES_H


namespace sharemind {

template <typename TmplInstance, typename ... Ts> struct TemplateAppendTypes;

template <typename TmplInstance, typename ... Ts>
using TemplateAppendTypes_t =
        typename TemplateAppendTypes<TmplInstance, Ts...>::type;

template <template <typename ...> class Tmpl,
          typename ... Ts,
          typename ... Ts2>
struct TemplateAppendTypes<Tmpl<Ts...>, Ts2...>
{ using type = Tmpl<Ts..., Ts2...>; };

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATEAPPENDTYPES_H */
