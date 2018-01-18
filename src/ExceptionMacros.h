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


#define SHAREMIND_DEFINE_EXCEPTION(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        name() noexcept(std::is_nothrow_default_constructible<base>::value) \
                = default; \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value) \
                = default; \
        ~name() noexcept override = default; \
        name & operator=(name const &) \
                noexcept(std::is_nothrow_copy_assignable<base>::value) \
                = default; \
    }

#define SHAREMIND_DECLARE_EXCEPTION_NOINLINE(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        name() noexcept(std::is_nothrow_default_constructible<base>::value); \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value); \
        ~name() noexcept override; \
        name & operator=(name const &) \
                noexcept(std::is_nothrow_copy_assignable<base>::value); \
    }
#define SHAREMIND_DEFINE_EXCEPTION_NOINLINE(base,ns,name) \
    ns name::name() \
            noexcept(std::is_nothrow_default_constructible<base>::value) \
            = default; \
    ns name::name(name const &) \
            noexcept(std::is_nothrow_copy_constructible<base>::value) \
            = default; \
    ns name::~name() noexcept = default; \
    ns name & ns name::operator=(name const &) \
            noexcept(std::is_nothrow_copy_assignable<base>::value) = default

#define SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(base,name,msg) \
    class name: public base { \
    public: /* Methods: */ \
        name() noexcept(std::is_nothrow_default_constructible<base>::value) \
                = default; \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value) \
                = default; \
        ~name() noexcept override = default; \
        name & operator=(name const &) \
                noexcept(std::is_nothrow_copy_assignable<base>::value) \
                = default; \
        const char * what() const noexcept final override \
        { return (msg); } \
    }
#define SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        name() noexcept(std::is_nothrow_default_constructible<base>::value); \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value); \
        ~name() noexcept override; \
        name & operator=(name const &) \
                noexcept(std::is_nothrow_copy_assignable<base>::value); \
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
    ns name & ns name::operator=(name const &) \
            noexcept(std::is_nothrow_copy_assignable<base>::value) = default; \
    const char * ns name::what() const noexcept { return (msg); }

#define SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        name(std::string message) \
            : m_message(std::make_shared<std::string>(std::move(message))) \
        {} \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value) \
                = default; \
        ~name() noexcept override = default; \
        name & operator=(name const &) \
                noexcept(std::is_nothrow_copy_assignable<base>::value) \
                = default; \
        const char * what() const noexcept final override { \
            assert(m_message); \
            return m_message->c_str(); \
        } \
    private: /* Methods: */ \
        std::shared_ptr<std::string const> m_message; \
    }

#define SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(base,name) \
    class name: public base { \
    public: /* Methods: */ \
        name(std::string message); \
        name(std::shared_ptr<std::string const> messagePtr) \
                noexcept(std::is_nothrow_default_constructible<base>::value); \
        name(name const &) \
                noexcept(std::is_nothrow_copy_constructible<base>::value); \
        name & operator=(name const &) \
                noexcept(std::is_nothrow_copy_assignable<base>::value); \
        char const * what() const noexcept final override; \
    private: /* Methods: */ \
        std::shared_ptr<std::string const> m_message; \
    }

#define SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(base,ns,name) \
    ns name::name(std::string message) \
        : m_message(std::make_shared<std::string>(std::move(message))) \
    {} \
    ns name::name(std::shared_ptr<std::string const> messagePtr) \
            noexcept(std::is_nothrow_default_constructible<base>::value) \
        : m_message(std::move(messagePtr)) \
    {} \
    ns name::name(name const &) \
            noexcept(std::is_nothrow_copy_constructible<base>::value) \
            = default; \
    ns name & ns name::operator=(name const &) \
            noexcept(std::is_nothrow_copy_assignable<base>::value) \
            = default; \
    char const * ns name::what() const noexcept { \
        assert(m_message); \
        return m_message->c_str(); \
    }

#endif /* SHAREMIND_EXCEPTIONMACROS_H */
