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

#ifndef SHAREMIND_TEMPLATEFILTER_H
#define SHAREMIND_TEMPLATEFILTER_H

#include <type_traits>
#include "TemplateList.h"
#include "TemplatePrependOne.h"


namespace sharemind {

template <typename T, template <T> class Filter, T ... vs>
struct TemplateFilter;

template <typename T, template <T> class Filter, T ... vs>
using TemplateFilter_t = typename TemplateFilter<T, Filter, vs...>::type;

template <typename T, template <T> class Filter>
struct TemplateFilter<T, Filter>
{ using type = TemplateList<T>; };

template <typename T, template <T> class Filter, T v, T ... vs>
struct TemplateFilter<T, Filter, v, vs...> {
    using type =
            typename std::conditional<
                Filter<v>::value,
                TemplatePrependOne_t<
                    T,
                    TemplateFilter_t<T, Filter, vs...>::type_t,
                    v
                >,
                TemplateFilter_t<T, Filter, vs...>
            >::type;
};

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATEFILTER_H */
