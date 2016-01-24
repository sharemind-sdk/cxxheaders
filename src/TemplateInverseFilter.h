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

#ifndef SHAREMIND_TEMPLATEINVERSEFILTER_H
#define SHAREMIND_TEMPLATEINVERSEFILTER_H

#include <type_traits>
#include "TemplateList.h"
#include "TemplateListPrependOne.h"


namespace sharemind {

template <typename T, template <T> class Filter, T ... vs>
struct TemplateInverseFilter;

template <typename T, template <T> class Filter, T ... vs>
using TemplateInverseFilter_t =
        typename TemplateInverseFilter<T, Filter, vs...>::type;

template <typename T, template <T> class Filter>
struct TemplateInverseFilter<T, Filter>
{ using type = TemplateList<T>; };

template <typename T, template <T> class Filter, T v, T ... vs>
struct TemplateInverseFilter<T, Filter, v, vs...> {
    using type =
            typename std::conditional<
                !Filter<v>::value,
                typename TemplateListPrependOne<
                    T,
                    typename TemplateInverseFilter<T, Filter, vs...>::type,
                    v
                >::type,
                typename TemplateInverseFilter<T, Filter, vs...>::type
            >::type;
};

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATEINVERSEFILTER_H */
