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

#ifndef SHAREMIND_TEMPLATELIST_H
#define SHAREMIND_TEMPLATELIST_H

#include <cstddef>
#include "EnumConstant.h"


namespace sharemind {

template <typename T, T ... Values> struct TemplateList {
    using type = TemplateList<T, Values...>;
    SHAREMIND_ENUMCONSTANT(std::size_t, size, sizeof...(Values));
};

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATELIST_H */
