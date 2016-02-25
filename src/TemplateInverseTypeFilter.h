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

#ifndef SHAREMIND_TEMPLATEINVERSETYPEFILTER_H
#define SHAREMIND_TEMPLATEINVERSETYPEFILTER_H

#include <type_traits>
#include "TemplatePrependOneType.h"
#include "TemplateTypeList.h"


namespace sharemind {

template <template <typename> class Filter, typename ... Ts>
struct TemplateInverseTypeFilter;

template <template <typename> class Filter, typename ... Ts>
using TemplateInverseTypeFilter_t =
        typename TemplateInverseTypeFilter<Filter, Ts...>::type;

template <template <typename> class Filter>
struct TemplateInverseTypeFilter<Filter>
{ using type = TemplateTypeList<>; };

template <template <typename> class Filter, typename T, typename ... Ts>
struct TemplateInverseTypeFilter<Filter, T, Ts...> {
    using type =
            typename std::conditional<
                !Filter<T>::value,
                TemplatePrependOneType_t<
                    typename TemplateInverseTypeFilter<Filter, Ts...>::type,
                    T
                >,
                typename TemplateInverseTypeFilter<Filter, Ts...>::type
            >::type;
};

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATEINVERSETYPEFILTER_H */
