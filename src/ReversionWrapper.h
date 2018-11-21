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

#ifndef SHAREMIND_REVERSIONWRAPPER_H
#define SHAREMIND_REVERSIONWRAPPER_H

namespace sharemind {

template <typename T>
struct ReversionWrapper { T & iterable; };

template <typename T>
auto begin(ReversionWrapper<T> const & rw)
        noexcept(noexcept(rw.iterable.rbegin()))
{ return rw.iterable.rbegin(); }

template <typename T>
auto end(ReversionWrapper<T> const & rw)
        noexcept(noexcept(rw.iterable.rend()))
{ return rw.iterable.rend(); }

template <typename T>
ReversionWrapper<T> makeReversionWrapper(T && iterable) noexcept
{ return { iterable }; }

} /* namespace Sharemind { */

#endif /* SHAREMIND_REVERSIONWRAPPER_H */
