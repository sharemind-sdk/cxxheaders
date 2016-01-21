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

#ifndef SHAREMIND_TEMPLATEAPPENDONE_H
#define SHAREMIND_TEMPLATEAPPENDONE_H


namespace sharemind {

template <typename T, template <T ...> class Tmpl, typename TmplInstance, T v>
struct TemplateAppendOne;

template <typename T, template <T ...> class Tmpl, typename TmplInstance, T v>
using TemplateAppendOne_t =
        typename TemplateAppendOne<T, Tmpl, TmplInstance, v>::type;

template <typename T, template <T ...> class Tmpl, T v, T ... vs>
struct TemplateAppendOne<T, Tmpl, Tmpl<vs...>, v>
{ using type = Tmpl<vs..., v>; };

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATEAPPENDONE_H */
