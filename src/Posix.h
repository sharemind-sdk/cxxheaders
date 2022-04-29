/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_POSIX_H
#define SHAREMIND_POSIX_H

#include <cerrno>
#include <fcntl.h>
#include <utility>
#include "Exception.h"
#include "ThrowNested.h"


namespace sharemind {

template <typename E, typename F, typename P, typename ... Args>
inline auto syscallLoop(F f, P okPred, Args && ... args) noexcept(false) {
    for (;;) {
        auto r = f(std::forward<Args>(args)...);
        if (okPred(r))
            return r;
        if (errno != EAGAIN && errno != EINTR)
            throwNested(ErrnoException(errno), E());
    }
}

namespace Detail {

template <int const GET, int const SET, typename GetE, typename SetE = GetE>
inline void fcntlAddFlags(int const fd, int const flags) noexcept(false) {
    syscallLoop<SetE>(
                ::fcntl,
                [](int const r) { return r == 0; },
                fd,
                SET,
                syscallLoop<GetE>(
                            ::fcntl,
                            [](int const r) { return r != -1; },
                            fd,
                            GET) | flags);
}

} /* namespace Detail { */

template <typename GetException, typename SetException = GetException>
inline void fcntlAddFd(int const fd, int const f) noexcept(false)
{ Detail::fcntlAddFlags<F_GETFD, F_SETFD, GetException, SetException>(fd, f); }

template <typename GetException, typename SetException = GetException>
inline void fcntlAddFl(int const fd, int const f) noexcept(false)
{ Detail::fcntlAddFlags<F_GETFL, F_SETFL, GetException, SetException>(fd, f); }

} /* namespace sharemind {*/

#endif /* SHAREMIND_POSIX_H */
