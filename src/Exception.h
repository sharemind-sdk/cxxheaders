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

#ifndef SHAREMIND_EXCEPTION_H
#define SHAREMIND_EXCEPTION_H

#include <array>
#include <cstddef>
#include <cstring>
#include <errno.h>
#include <exception>
#if ! (defined(__APPLE__) || defined(__FreeBSD__))
#include <features.h>
#endif
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/cdefs.h>
#endif
#include "detail/ExceptionMacros.h"


namespace sharemind {

#if (_POSIX_C_SOURCE >= 200112L \
     || _XOPEN_SOURCE >= 600 \
     || __APPLE__ \
     || __FreeBSD__) \
    && ! _GNU_SOURCE
    #define SHAREMIND_STRERROR_(e, buf, bufsize) \
        do { \
            const int r = strerror_r((e), (buf), (bufsize)); \
            if (r != 0) { \
                static const char errorMsg[] = "unknown: strerror_r error"; \
                /* snprintf present, because this holds:
                 _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE
                 || _POSIX_C_SOURCE >= 200112L */ \
                snprintf((buf), (bufsize), "%s %d", errorMsg, \
                         (r > 0 ? r : errno)); \
            } \
        } while(0)
#else
    #define SHAREMIND_STRERROR_(e, buf, bufsize) \
        do { \
            char * const c = strerror_r((e), (buf), (bufsize)); \
            if (c != buf) { \
                auto const len = std::strlen(c); \
                if (len < (bufsize)) { \
                    std::memmove((buf), c, len + 1u); \
                } else { \
                    std::memmove((buf), c, (bufsize) - 1u); \
                    (buf)[(bufsize) - 1u] = '\0'; \
                } \
            } \
        } while(0)
#endif

SHAREMIND_DETAIL_DEFINE_EXCEPTION(std::exception, Exception);

class ErrnoException: public Exception {

public: /* Constants: */

    constexpr static std::size_t BUFFER_SIZE = 1024u;

private: /* Types: */

    using MessageBuffer = std::array<char, BUFFER_SIZE>;

public: /* Methods: */

    ErrnoException(ErrnoException const &) = default;
    ErrnoException & operator=(ErrnoException const &) = default;
    ErrnoException(ErrnoException &&) = default;
    ErrnoException & operator=(ErrnoException &&) = default;

    explicit ErrnoException(int const e) noexcept
        : m_message(
            [e]() noexcept {
                MessageBuffer r;
                SHAREMIND_STRERROR_(e, r.data(), BUFFER_SIZE);
                return r;
            }())
        , m_errno(e)
    {}

    char const * what() const noexcept final override
    { return m_message.data(); }

    int errorNumber() const noexcept { return m_errno; }

private: /* Fields: */

    MessageBuffer m_message;
    int m_errno;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_EXCEPTION_H */
