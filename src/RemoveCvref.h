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

#ifndef SHAREMIND_REMOVECVREF_H
#define SHAREMIND_REMOVECVREF_H

#include <type_traits>

namespace sharemind {

#if __cplusplus > 201703L
template <typename T> using RemoveCvref = std::remove_cvref<T>;
template <typename T> using RemoveCvrefT = typename std::remove_cvref<T>::type;
#else
template <typename T> using RemoveCvref =
        std::remove_cv<typename std::remove_reference<T>::type>;
template <typename T> using RemoveCvrefT = typename RemoveCvref<T>::type;
#endif

} /* namespace Sharemind { */

#endif /* SHAREMIND_REMOVECVREF_H */
