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

#ifndef SHAREMIND_TEMPLATECOMMONPREFIXTYPES_H
#define SHAREMIND_TEMPLATECOMMONPREFIXTYPES_H

#include <type_traits>
#include "TemplateTypeList.h"


namespace sharemind {
namespace Detail {

template <typename Accum, typename T, typename T2>
struct TemplateCommonPrefixTypes_;

template <template <typename ...> class Accum,
          template <typename ...> class T,
          template <typename ...> class T2,
          typename ... AccumTs>
struct TemplateCommonPrefixTypes_<Accum<AccumTs...>, T<>, T2<> >
{ using type = Accum<AccumTs...>; };

template <template <typename ...> class Accum,
          template <typename ...> class T,
          template <typename ...> class T2,
          typename ... AccumTs,
          typename TT,
          typename ... TTs>
struct TemplateCommonPrefixTypes_<Accum<AccumTs...>, T<TT, TTs...>, T2<> >
{ using type = Accum<AccumTs...>; };

template <template <typename ...> class Accum,
          template <typename ...> class T,
          template <typename ...> class T2,
          typename ... AccumTs,
          typename T2T,
          typename ... T2Ts>
struct TemplateCommonPrefixTypes_<Accum<AccumTs...>, T<>, T2<T2T, T2Ts...> >
{ using type = Accum<AccumTs...>; };

template <template <typename ...> class Accum,
          template <typename ...> class T,
          template <typename ...> class T2,
          typename ... AccumTs,
          typename TT,
          typename ... TTs,
          typename T2T,
          typename ... T2Ts>
struct TemplateCommonPrefixTypes_<Accum<AccumTs...>,
                                 T<TT, TTs...>,
                                 T2<T2T, T2Ts...> >
{
    using type =
            typename std::conditional<
                std::is_same<TT, T2T>::value,
                typename TemplateCommonPrefixTypes_<
                    Accum<AccumTs..., TT>,
                    T<TTs...>,
                    T2<T2Ts...>
                >::type,
                Accum<AccumTs...>
            >::type;
};

} /* namespace Detail { */

template <typename ... Ts>
struct TemplateCommonPrefixTypes;

template <>
struct TemplateCommonPrefixTypes<> { using type = TemplateTypeList<>; };

template <template <typename...> class T, typename ... Ts>
struct TemplateCommonPrefixTypes<T<Ts...> >
{ using type = TemplateTypeList<Ts...>; };

template <template <typename...> class T,
          template <typename...> class T2,
          typename ... Ts,
          typename ... Ts2,
          typename ... Ts3>
struct TemplateCommonPrefixTypes<T<Ts...>, T2<Ts2...>, Ts3...> {
    using type =
            typename TemplateCommonPrefixTypes<
                typename Detail::TemplateCommonPrefixTypes_<
                    TemplateTypeList<>,
                    T<Ts...>,
                    T2<Ts2...>
                >::type,
                Ts3...
            >::type;
};

template <typename ... Ts>
using TemplateCommonPrefixTypes_t =
        typename TemplateCommonPrefixTypes<Ts...>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATECOMMONPREFIXTYPES_H */
