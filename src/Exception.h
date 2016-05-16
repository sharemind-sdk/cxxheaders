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

#ifndef SHAREMIND_EXCEPTION_H
#define SHAREMIND_EXCEPTION_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <errno.h>
#include <exception>
#if ! (defined(__APPLE__) || defined(__FreeBSD__))
#include <features.h>
#endif
#include <string>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/cdefs.h>
#endif
#include <utility>
#include "compiler-support/GccNoreturn.h"
#include "EnumConstant.h"


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

#define SHAREMIND_DEFINE_EXCEPTION(base,name) \
    class name: public base {}

#define SHAREMIND_DEFINE_EXCEPTION_UNUSED(base,name) \
    class name: public base { \
    private: /* Methods: */ \
        name() = delete; \
        name(name &&) = delete; \
        name(name const &) = delete; \
        name & operator=(name &&) = delete; \
        name & operator=(name const &) = delete; \
    }


#define SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(base,name,msg) \
    class name: public base { \
    public: /* Methods: */ \
        template <typename ... Args> \
        inline name(Args && ... args) : base(std::forward<Args>(args)...) {} \
        inline const char * what() const noexcept final override \
        { return (msg); } \
    }

#define SHAREMIND_DEFINE_EXCEPTION_CONCAT(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        inline name(const char * const msg) : m_msgPtr((assert(msg), msg)) {} \
        template <typename Arg, typename ... Args> \
        inline name(const char * const defaultMsg, \
                    Arg && arg, Args && ... args) \
        { \
            try { \
                m_msg.assign(::sharemind::concat(std::forward<Arg>(arg), \
                                                 std::forward<Args>(args)...));\
                m_msgPtr = m_msg.c_str(); \
            } catch (...) { \
                m_msgPtr = defaultMsg; \
            } \
        } \
        inline const char * what() const noexcept final override \
        { return m_msgPtr; } \
        std::string m_msg; \
        const char * m_msgPtr; \
    }

SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);

class ErrnoException: public Exception {

public: /* Constants: */

    SHAREMIND_ENUMCONSTANT(std::size_t, BUFFER_SIZE, 1024u);

public: /* Methods: */

    ErrnoException(const ErrnoException &) = default;
    ErrnoException & operator=(const ErrnoException &) = default;
    ErrnoException(ErrnoException &&) = default;
    ErrnoException & operator=(ErrnoException &&) = default;

    explicit inline ErrnoException(const int e) noexcept
    { SHAREMIND_STRERROR_(e, m_message, BUFFER_SIZE); }

    inline const char * what() const noexcept final override
    { return m_message; }

    template <typename Exception_>
    SHAREMIND_GCC_NORETURN_PART1
    static void throwAsNestedOf(Exception_ && e,
                                const int returnStatus = errno) noexcept(false)
            SHAREMIND_GCC_NORETURN_PART2;

    template <typename Exception_>
    SHAREMIND_GCC_NORETURN_PART1
    static void throwAsNestedOf(const int returnStatus = errno) noexcept(false)
            SHAREMIND_GCC_NORETURN_PART2;

private: /* Methods: */

    char m_message[BUFFER_SIZE];

};

template <typename Exception_>
void ErrnoException::throwAsNestedOf(Exception_ && e,
                                     const int c) noexcept(false)
{
    try { throw ErrnoException(c); }
    catch (...) { std::throw_with_nested(std::forward<Exception_>(e)); }
}

template <typename Exception_>
void ErrnoException::throwAsNestedOf(const int c) noexcept(false)
{
    try { throw ErrnoException(c); }
    catch (...) { std::throw_with_nested(Exception_()); }
}

} /* namespace sharemind { */

#endif /* SHAREMIND_EXCEPTION_H */
