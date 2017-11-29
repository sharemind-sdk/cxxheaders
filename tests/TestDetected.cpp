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

#include "../src/Detected.h"

#include <type_traits>


using sharemind::NoneSuch;
using sharemind::DetectedOr;
using sharemind::DetectedOrT;
using sharemind::IsDetected;
using sharemind::DetectedT;
using sharemind::IsDetectedExact;
using sharemind::IsDetectedConvertible;

struct Y;
struct Z;
struct XB {};
struct X: XB { Y const & returnRef(Z const * x) const; };

template <typename T>
using OK =
        decltype(std::declval<T const &>().returnRef(
                std::declval<Z const *>()));
template <typename T>
using NOK =
        decltype(std::declval<T const &>().returnRef(std::declval<Y &>()));

using DOXO = DetectedOr<NoneSuch, OK, X>;
using DOXN = DetectedOr<NoneSuch, NOK, X>;
using DOTXO = DetectedOrT<NoneSuch, OK, X>;
using DOTXN = DetectedOrT<NoneSuch, NOK, X>;
using IDXO = IsDetected<OK, X>;
using IDXN = IsDetected<NOK, X>;
using DXO = DetectedT<OK, X>;
using DXN = DetectedT<NOK, X>;
template <typename T> using IDEXO = IsDetectedExact<T, OK, X>;
template <typename T> using IDEXN = IsDetectedExact<T, NOK, X>;
template <typename T> using IDCXO = IsDetectedConvertible<T, OK, X>;
template <typename T> using IDCXN = IsDetectedConvertible<T, NOK, X>;

static_assert(std::is_same<DOXO::ValueType, std::true_type>::value, "");
static_assert(std::is_same<DOXO::Type, Y const &>::value, "");
static_assert(std::is_same<DOXN::ValueType, std::false_type>::value, "");
static_assert(std::is_same<DOXN::Type, NoneSuch>::value, "");
static_assert(std::is_same<DOTXO, DOXO::Type>::value, "");
static_assert(std::is_same<DOTXN, DOXN::Type>::value, "");
static_assert(std::is_same<IDXO, DOXO::ValueType>::value, "");
static_assert(std::is_same<IDXN, DOXN::ValueType>::value, "");
static_assert(std::is_same<DXO, DOXO::Type>::value, "");
static_assert(std::is_same<DXN, DOXN::Type>::value, "");
static_assert(std::is_same<IDEXO<X>, std::is_same<X, DXO> >::value, "");
static_assert(std::is_same<IDEXN<X>, std::is_same<X, DXN> >::value, "");
static_assert(std::is_same<IDCXO<XB>, std::is_convertible<DXO, XB> >::value,"");
static_assert(std::is_same<IDCXN<XB>, std::is_convertible<DXN, XB> >::value,"");

int main() {}
