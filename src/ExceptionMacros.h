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

#ifndef SHAREMIND_EXCEPTIONMACROS_H
#define SHAREMIND_EXCEPTIONMACROS_H

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include "Concat.h"


#define SHAREMIND_DEFINE_EXCEPTION(base,name) \
    class name: public base {}

#define SHAREMIND_DECLARE_EXCEPTION_NOINLINE(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        name() noexcept(std::is_nothrow_default_constructible<base>::value); \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value); \
        ~name() noexcept override; \
    }
#define SHAREMIND_DEFINE_EXCEPTION_NOINLINE(base,ns,name) \
    ns name::name() \
            noexcept(std::is_nothrow_default_constructible<base>::value) \
            = default; \
    ns name::name(name const &) \
            noexcept(std::is_nothrow_copy_constructible<base>::value) \
            = default; \
    ns name::~name() noexcept = default

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
#define SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        name() noexcept(std::is_nothrow_default_constructible<base>::value); \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value); \
        ~name() noexcept override; \
        template <typename ... Args> \
        name(Args && ... args) \
                noexcept(std::is_nothrow_constructible<base, Args...>::value) \
            : base(std::forward<Args>(args)...) {} \
        const char * what() const noexcept final override; \
    }
#define SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(base,ns,name,msg) \
    ns name::name() \
            noexcept(std::is_nothrow_default_constructible<base>::value) \
            = default; \
    ns name::name(name const &) \
            noexcept(std::is_nothrow_copy_constructible<base>::value) \
            = default; \
    ns name::~name() noexcept = default; \
    const char * ns name::what() const noexcept { return (msg); }

#define SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        inline name(std::string message) \
            : m_message(std::make_shared<std::string>(std::move(message))) \
        {} \
        inline const char * what() const noexcept final override { \
            assert(m_message); \
            return m_message->c_str(); \
        } \
    private: /* Methods: */ \
        std::shared_ptr<std::string const> m_message; \
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

#endif /* SHAREMIND_EXCEPTIONMACROS_H */
