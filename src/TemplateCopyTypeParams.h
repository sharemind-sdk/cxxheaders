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

#ifndef SHAREMIND_TEMPLATECOPYTYPEPARAMS_H
#define SHAREMIND_TEMPLATECOPYTYPEPARAMS_H

namespace sharemind {

template <typename From, template <typename ...> class To>
struct TemplateCopyTypeParams;

template <template <typename ...> class From,
          template <typename ...> class To,
          typename ... Ts>
struct TemplateCopyTypeParams<From<Ts...>, To>
{ using type = To<Ts...>; };

template <typename From, template <typename ...> class To>
using TemplateCopyTypeParams_t = typename TemplateCopyTypeParams<From, To>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATECOPYTYPEPARAMS_H */
