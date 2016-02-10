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

#ifndef SHAREMIND_TEMPLATEGETTYPEPARAM_H
#define SHAREMIND_TEMPLATEGETTYPEPARAM_H

#include <cstddef>


namespace sharemind {

template <std::size_t I, typename ... Ts>
struct TemplateGetTypeParam;

template <typename T, typename ... Ts>
struct TemplateGetTypeParam<0u, T, Ts...> { using type = T; };

template <std::size_t I, typename T, typename ... Ts>
struct TemplateGetTypeParam<I, T, Ts...>
{ using type = typename TemplateGetTypeParam<I - 1u, Ts...>::type; };

template <std::size_t I, typename ... Ts>
using TemplateGetTypeParam_t = typename TemplateGetTypeParam<I, Ts...>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATEGETTYPEPARAM_H */
