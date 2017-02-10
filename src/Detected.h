/*
 * Copyright (C) 2017 Cybernetica
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

#ifndef SHAREMIND_DETECTED_H
#define SHAREMIND_DETECTED_H

#include <type_traits>
#include "Void.h"

namespace sharemind {
namespace Detail {

template <typename Default,
          typename, // For Void_t
          template <typename ...> class Op,
          typename ... Args>
struct Detector {
    using ValueType = std::false_type;
    using Type = Default;
};

template <typename Default,
          template <typename ...> class Op,
          typename ... Args>
struct Detector<Default, Void_t<Op<Args...> >, Op, Args...> {
    using ValueType = std::true_type;
    using Type = Op<Args...>;
};

} /* namespace Detail { */

struct NoneSuch {
    NoneSuch() = delete;
    ~NoneSuch() = delete;
    NoneSuch(NoneSuch const &) = delete;
    NoneSuch & operator=(NoneSuch const &) = delete;
};

template <typename Default, template <class ...> class Op, class ... Args>
using DetectedOr = Detail::Detector<Default, void, Op, Args...>;

template <typename Default, template <class ...> class Op, class ... Args>
using DetectedOrT = typename Detail::Detector<Default, void, Op, Args...>::Type;

template <template <class ...> class Op, class ... Args>
using IsDetected = typename Detail::Detector<int, void, Op, Args...>::ValueType;

template <template <class ...> class Op, class ... Args>
using DetectedT = typename Detail::Detector<NoneSuch, void, Op, Args...>::Type;

template <typename Expected, template <class ...> class Op, class ... Args>
using IsDetectedExact = std::is_same<Expected, DetectedT<Op, Args...> >;

template <typename To, template <class ...> class Op, class ... Args>
using IsDetectedConvertible = std::is_convertible<DetectedT<Op, Args...>, To>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_DETECTED_H */
