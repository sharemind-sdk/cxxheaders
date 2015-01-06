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

#ifndef SHAREMIND_SCOPEEXIT_H
#define SHAREMIND_SCOPEEXIT_H

namespace sharemind {

template <typename F>
class ScopeExit {

public: /* Methods: */

    inline ScopeExit(F f) : m_f(f) {}
    inline ~ScopeExit() noexcept { m_f(); }

private: /* Fields: */

    F m_f;

};

template <typename F>
inline ScopeExit<F> makeScopeExit(F f) { return ScopeExit<F>(f); }

#define SHAREMIND_SCOPE_EXIT_CAT(a,b) SHAREMIND_SCOPE_EXIT_CAT2(a,b)
#define SHAREMIND_SCOPE_EXIT_CAT2(a,b) a ## b

#define SHAREMIND_SCOPE_EXIT(...) \
    auto const SHAREMIND_SCOPE_EXIT_CAT(scopeExit_,__LINE__) = \
            ::sharemind::makeScopeExit([=](){ __VA_ARGS__ ; })

} /* namespace sharemind { */

#endif /* SHAREMIND_SCOPEEXIT_H */
