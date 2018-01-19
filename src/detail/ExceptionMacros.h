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

#ifndef SHAREMIND_DETAIL_EXCEPTIONMACROS_H
#define SHAREMIND_DETAIL_EXCEPTIONMACROS_H

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include "../compiler-support/ClangPR23383.h"


#define SHAREMIND_DETAIL_DEFINE_EXCEPTION(base,name) \
    class name: public base { \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowDefaultConstructible, \
            std::is_nothrow_default_constructible<base>::value) \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowCopyConstructible, \
            std::is_nothrow_copy_constructible<base>::value) \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowCopyAssignable, \
            std::is_nothrow_copy_assignable<base>::value) \
    public: /* Methods: */ \
        name() noexcept( \
                    SHAREMIND_CLANGPR23383_WORKAROUND( \
                        baseIsNothrowDefaultConstructible, \
                        std::is_nothrow_default_constructible<base>::value)) \
                = default; \
        name(name const &) \
                noexcept( \
                    SHAREMIND_CLANGPR23383_WORKAROUND( \
                        baseIsNothrowCopyConstructible, \
                        std::is_nothrow_copy_constructible<base>::value)) \
                = default; \
        ~name() noexcept override = default; \
        name & operator=(name const &) \
                noexcept( \
                    SHAREMIND_CLANGPR23383_WORKAROUND( \
                        baseIsNothrowCopyAssignable, \
                        std::is_nothrow_copy_assignable<base>::value)) \
                = default; \
    }

#define SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(base,name,msg) \
    class name: public base { \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowDefaultConstructible, \
            std::is_nothrow_default_constructible<base>::value) \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowCopyConstructible, \
            std::is_nothrow_copy_constructible<base>::value) \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowCopyAssignable, \
            std::is_nothrow_copy_assignable<base>::value) \
    public: /* Methods: */ \
        name() noexcept( \
                SHAREMIND_CLANGPR23383_WORKAROUND( \
                    baseIsNothrowDefaultConstructible, \
                    std::is_nothrow_default_constructible<base>::value)) \
                = default; \
        name(name const &) \
                noexcept( \
                    SHAREMIND_CLANGPR23383_WORKAROUND( \
                        baseIsNothrowCopyConstructible, \
                        std::is_nothrow_copy_constructible<base>::value)) \
                = default; \
        ~name() noexcept override = default; \
        name & operator=(name const &) \
                noexcept( \
                    SHAREMIND_CLANGPR23383_WORKAROUND( \
                        baseIsNothrowCopyAssignable, \
                        std::is_nothrow_copy_assignable<base>::value)) \
                = default; \
        const char * what() const noexcept final override \
        { return (msg); } \
    }

#define SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_STDSTRING(base,name) \
    class name: public base { \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowDefaultConstructible, \
            std::is_nothrow_default_constructible<base>::value) \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowCopyConstructible, \
            std::is_nothrow_copy_constructible<base>::value) \
        SHAREMIND_CLANGPR23383_WORKAROUND_STATIC_PRIVATE_CONSTEXPR_DEF( \
            baseIsNothrowCopyAssignable, \
            std::is_nothrow_copy_assignable<base>::value) \
    public: /* Methods: */ \
        name(std::string message) \
            : m_message(std::make_shared<std::string>(std::move(message))) \
        {} \
        name(name const &) \
                noexcept( \
                    SHAREMIND_CLANGPR23383_WORKAROUND( \
                        baseIsNothrowCopyConstructible, \
                        std::is_nothrow_copy_constructible<base>::value)) \
                = default; \
        ~name() noexcept override = default; \
        name & operator=(name const &) \
                noexcept( \
                    SHAREMIND_CLANGPR23383_WORKAROUND( \
                        baseIsNothrowCopyAssignable, \
                        std::is_nothrow_copy_assignable<base>::value)) \
                = default; \
        const char * what() const noexcept final override { \
            assert(m_message); \
            return m_message->c_str(); \
        } \
    private: /* Methods: */ \
        std::shared_ptr<std::string const> m_message; \
    }

#endif /* SHAREMIND_DETAIL_EXCEPTIONMACROS_H */
