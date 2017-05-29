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

#ifndef SHAREMIND_TEMPLATETYPELIST_H
#define SHAREMIND_TEMPLATETYPELIST_H

#include <tuple>


namespace sharemind {

template <typename ... Ts> struct TemplateTypeList {
    using AsTuple = std::tuple<Ts...>;
    static constexpr decltype(sizeof...(Ts)) size() { return sizeof...(Ts); }

    template <std::size_t N>
    using type = typename std::tuple_element<N, AsTuple>::type;
};

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATETYPELIST_H */
