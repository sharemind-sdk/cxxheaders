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

#ifndef SHAREMIND_ISREFERENCEABLE_H
#define SHAREMIND_ISREFERENCEABLE_H

#include <type_traits>


namespace sharemind {
namespace Detail {
namespace IsReferenceable {

template <typename T_, typename = T_ &> inline std::true_type test(int);
template <typename> inline std::false_type test(...);

} /* namespace IsReferenceable { */
} /* namespace Detail { */

template <typename T>
struct IsReferenceable
        : std::integral_constant<
                bool,
                decltype(Detail::IsReferenceable::test<T>(0))::value
            >
{};

} /* namespace Sharemind { */

#endif /* SHAREMIND_ISREFERENCEABLE_H */
