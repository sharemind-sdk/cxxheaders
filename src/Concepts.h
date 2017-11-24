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

#ifndef SHAREMIND_CONCEPTS_H
#define SHAREMIND_CONCEPTS_H

#include <type_traits>
#include <utility>
#include "TemplateAll.h"
#include "Void.h"


namespace sharemind {

struct Concept {};

template <typename ...> struct ValidTypes;

namespace Detail {

enum class RequiresConceptResult { Succeed };

template <typename Concept, typename = void>
struct ModelsConcept: std::false_type {};

template <typename Concept, typename ... Ts>
struct ModelsConcept<
        Concept(Ts...),
        Void_t<decltype(std::declval<Concept>().check(std::declval<Ts>()...))>
    > : std::true_type
{};

} /* namespace Detail { */

template <typename ... Cs>
using Models = TemplateAll<Detail::ModelsConcept<Cs>::value...>;

#define SHAREMIND_REQUIRE(...) \
    typename ::std::enable_if< \
        __VA_ARGS__, \
        ::sharemind::Detail::RequiresConceptResult \
    >::type

#define SHAREMIND_REQUIRE_DEFAULTVALUE \
    ::sharemind::Detail::RequiresConceptResult::Succeed

#define SHAREMIND_REQUIRE_CONCEPT(...) \
    SHAREMIND_REQUIRE(Models<__VA_ARGS__>::value)

#define SHAREMIND_REQUIRES(...) \
    SHAREMIND_REQUIRE(__VA_ARGS__) = SHAREMIND_REQUIRE_DEFAULTVALUE

#define SHAREMIND_REQUIRES_CONCEPT(...) \
    SHAREMIND_REQUIRE_CONCEPT(__VA_ARGS__) = SHAREMIND_REQUIRE_DEFAULTVALUE

#define SHAREMIND_DEFINE_CONCEPT(name) struct name: ::sharemind::Concept

} /* namespace Sharemind { */

#endif /* SHAREMIND_CONCEPTS_H */
