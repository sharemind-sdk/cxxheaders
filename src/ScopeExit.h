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

#ifndef SHAREMIND_SCOPEEXIT_H
#define SHAREMIND_SCOPEEXIT_H

#include <functional>
#include <utility>


namespace sharemind {

class ScopeExit {

public: /* Methods: */

    template <typename ... Args>
    ScopeExit(Args && ... args) : m_f{std::forward<Args>(args)...} {}
    ~ScopeExit() noexcept(false) { m_f(); }

private: /* Fields: */

    std::function<void()> m_f;

};

template <typename F>
[[deprecated]] inline ScopeExit makeScopeExit(F && f);

template <typename F>
inline ScopeExit makeScopeExit(F && f)
{ return ScopeExit{std::forward<F>(f)}; }

#define SHAREMIND_SCOPE_EXIT_CAT(a,b) SHAREMIND_SCOPE_EXIT_CAT2(a,b)
#define SHAREMIND_SCOPE_EXIT_CAT2(a,b) a ## b

#define SHAREMIND_SCOPE_EXIT(...) \
    ::sharemind::ScopeExit const SHAREMIND_SCOPE_EXIT_CAT(scopeExit_,__LINE__){\
            [=](){ __VA_ARGS__ ; }}

} /* namespace sharemind { */

#endif /* SHAREMIND_SCOPEEXIT_H */
