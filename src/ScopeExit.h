/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMINDCOMMON_SCOPEEXIT_H
#define SHAREMINDCOMMON_SCOPEEXIT_H

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

#endif /* SHAREMINDCOMMON_SCOPEEXIT_H */
