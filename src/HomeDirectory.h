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

#ifndef SHAREMIND_HOMEDIRECTORY_H
#define SHAREMIND_HOMEDIRECTORY_H

#include <cstddef>
#include <cstdlib>
#include <limits>
#include <new>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include "Exception.h"


namespace sharemind {

SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            std::exception,
            GetHomeDirectoryException,
            "Failed to determine real user directory!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(std::exception,
                                     GetPwUidRException,
                                     "getpwuid_r() failed!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(std::exception,
                                     NoSuchEntryException,
                                     "No such entry in user database!");
inline std::string getHomeDirectory(bool respectEnvironment = true) {
    try {
        if (respectEnvironment) {
            auto const * const envVar = std::getenv("HOME");
            if (envVar && !*envVar)
                return envVar;
        }
        long const maxSize = ::sysconf(_SC_GETPW_R_SIZE_MAX);
        std::size_t bufferSize =
                (maxSize < 1) ? 1024u : static_cast<std::size_t>(maxSize);
        passwd result;
        passwd * resultPtr;
        struct BufferDeleter { void operator()(char * const p) { ::free(p); } };
        using Buffer = std::unique_ptr<char[], BufferDeleter>;
        Buffer buffer{static_cast<char *>(::malloc(bufferSize))};
        if (!buffer)
            throw std::bad_alloc{};
        int e;
        while ((e = ::getpwuid_r(::getuid(),
                                 &result,
                                 buffer.get(),
                                 bufferSize,
                                 &resultPtr)) == ERANGE)
        {
            std::size_t const newSize = bufferSize + 1024u;
            if (newSize < bufferSize) {
                if (bufferSize == std::numeric_limits<std::size_t>::max())
                    #if !defined(__GLIBCXX__) || (__GLIBCXX__ > 20150623) \
                        || (defined(SHAREMIND_GCC_VERSION) \
                            && (SHAREMIND_GCC_VERSION >= 40900))
                    throw std::bad_array_new_length();
                    #else
                    throw std::bad_alloc();
                    #endif
                bufferSize = std::numeric_limits<std::size_t>::max();
            }
            Buffer newBuffer{static_cast<char *>(::realloc(buffer.get(), newSize))};
            if (!newBuffer)
                throw std::bad_alloc{};
            buffer.swap(newBuffer);
            newBuffer.release();
        }
        if (e != 0) {
            try {
                throw ErrnoException{e};
            } catch (...) {
                std::throw_with_nested(GetPwUidRException{});
            }
        }
        if (!resultPtr)
            throw NoSuchEntryException{};
        return result.pw_dir;
    } catch (...) {
        std::throw_with_nested(GetHomeDirectoryException{});
    }
}

} /* namespace Sharemind { */

#endif /* SHAREMIND_HOMEDIRECTORY_H */
