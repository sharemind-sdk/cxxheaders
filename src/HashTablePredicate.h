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

#ifndef SHAREMIND_HASHTABLEPREDICATE_H
#define SHAREMIND_HASHTABLEPREDICATE_H

#include <cstddef>
#include <type_traits>
#include "Concepts.h"


namespace sharemind {

template <typename Key>
SHAREMIND_DEFINE_CONCEPT(HashTablePredicate) {
    template <typename Pred>
    auto check(Pred && pred) -> SHAREMIND_REQUIRE_CONCEPTS(
                UnaryPredicate(Pred, Key &),
                UnaryPredicate(Pred, Key const &),
                ConvertibleTo(
                    typename std::remove_cv<
                        typename std::remove_reference<
                            decltype(pred.hash())
                        >::type
                     >::type, std::size_t)
            );
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_HASHTABLEPREDICATE_H */
