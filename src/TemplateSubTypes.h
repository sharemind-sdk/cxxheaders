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

#ifndef SHAREMIND_TEMPLATESUBTYPES_H
#define SHAREMIND_TEMPLATESUBTYPES_H

#include <cstddef>
#include "TemplatePrependOneType.h"
#include "TemplateTypeList.h"


namespace sharemind {

template <std::size_t S, std::size_t N, typename ... Ts>
struct TemplateSubTypes;

template <std::size_t S, std::size_t N, typename ... Ts>
using TemplateSubTypes_t = typename TemplateSubTypes<S, N, Ts...>::type;

template <>
struct TemplateSubTypes<0u, 0u> { using type = TemplateTypeList<>; };

template <typename T>
struct TemplateSubTypes<0u, 0u, T> { using type = TemplateTypeList<>; };

template <typename T, typename ... Ts>
struct TemplateSubTypes<0u, 0u, T, Ts...> { using type = TemplateTypeList<>; };

template <std::size_t N, typename T, typename ... Ts>
struct TemplateSubTypes<0u, N, T, Ts...> {
    using type =
            typename TemplatePrependOneType<
                TemplateSubTypes_t<0u, N - 1u, Ts...>,
                T
            >::type;
};

template <std::size_t S, std::size_t N, typename T, typename ... Ts>
struct TemplateSubTypes<S, N, T, Ts...>: TemplateSubTypes<S - 1u, N, Ts...> {};

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATESUBTYPES_H */
