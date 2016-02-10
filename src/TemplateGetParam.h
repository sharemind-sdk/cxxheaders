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

#ifndef SHAREMIND_TEMPLATEGETPARAM_H
#define SHAREMIND_TEMPLATEGETPARAM_H

#include <cstddef>
#include <type_traits>


namespace sharemind {

template <typename T, std::size_t I, T ... vs>
struct TemplateGetParam;

template <typename T, T v, T ... vs>
struct TemplateGetParam<T, 0u, v, vs...>: std::integral_constant<T, v> {};

template <typename T, std::size_t I, T v, T ... vs>
struct TemplateGetParam<T, I, v, vs...>: TemplateGetParam<T, I - 1u, vs...> {};

template <typename T, std::size_t I, T ... vs>
using TemplateGetParam_t = typename TemplateGetParam<T, I, vs...>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATEGETPARAM_H */
