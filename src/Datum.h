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

#ifndef SHAREMIND_DATUM
#define SHAREMIND_DATUM

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <errno.h>
#include <fcntl.h>
#include <limits>
#include <sharemind/DebugOnly.h>
#include <string>
#include <sys/stat.h>
#include <type_traits>
#include <utility>
#include <vector>
#include "Concat.h"
#include "detail/ExceptionMacros.h"
#include "Exception.h"
#include "Hash.h"
#include "PotentiallyVoidTypeInfo.h"


namespace sharemind {

class Datum {

private: /* Types: */

    using Container = std::vector<char>;

public: /* Types: */

    using size_type = Container::size_type;
    using value_type = Container::value_type;

    SHAREMIND_DETAIL_DEFINE_EXCEPTION(sharemind::Exception, Exception);
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_STDSTRING(Exception, LoadException);
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                FileTypeNotSupportedException,
                                                "File type not supported!");
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                FileSizeChangedException,
                                                "File size changed!");

public: /* Methods: */

    Datum() noexcept(std::is_nothrow_default_constructible<Container>::value)
    {}

    Datum(Datum && move)
            noexcept(std::is_nothrow_move_constructible<Container>::value)
        : m_data(std::move(move.m_data)) {}

    Datum(Datum const & copy) : m_data(copy.m_data) {}

    explicit Datum(size_type size, value_type initial = value_type())
        : m_data(size, initial)
    {}

    Datum(void const * const data, size_type const size)
        : m_data(static_cast<value_type const *>(data),
                 static_cast<value_type const *>(data) + size)
    {}

    explicit Datum(std::string const & filename)
        : m_data(loadFileToContainer(filename))
    {}

    Datum & operator=(Datum const & copy) {
        m_data = copy.m_data;
        return *this;
    }

    Datum & operator=(Datum && move)
            noexcept(std::is_nothrow_move_assignable<Container>::value)
    {
        m_data = std::move(move.m_data);
        return *this;
    }

    bool operator==(Datum const & rhs) const noexcept
    { return m_data == rhs.m_data; }

    bool operator!=(Datum const & rhs) const noexcept
    { return m_data != rhs.m_data; }

    void resize(size_type const size, value_type initial = value_type())
    { m_data.resize(size, initial); }

    void loadFromFile(std::string const & filename)
    { m_data = loadFileToContainer(filename); }

    void assign(void const * const data, size_type const size) {
        m_data.resize(size);
        std::copy(static_cast<char const *>(data),
                  static_cast<char const *>(data) + size,
                  &m_data[0]);
    }

    HashValue hash() const noexcept
    { return hashRawData(&m_data[0u], m_data.size()); }

    void clear() noexcept { m_data.clear(); }
    bool empty() const noexcept { return m_data.empty(); }
    size_type size() const noexcept { return m_data.size(); }
    void * data() noexcept { return &m_data[0u]; }
    void * dataEnd() noexcept { return &*m_data.end(); }
    void const * constData() const noexcept { return &m_data[0u]; }
    void const * constDataEnd() const noexcept
    { return &*m_data.end(); }

private: /* Methods: */

    static Container loadFileToContainer(std::string const & filename) {
        LoadException loadException(
                    concat("Failed to load file \"", filename, "\"!"));
        Container contents;
        int const inFd = ::open(filename.c_str(), O_RDONLY | O_CLOEXEC);
        try {
            if (inFd < 0) {
                assert(inFd == -1);
                throw ErrnoException(errno);
            }

            std::size_t leftToRead =
                    [](int const fd) -> std::size_t {
                        struct ::stat statBuf;
                        if (SHAREMIND_DEBUG_ONLY(auto const r =)
                                ::fstat(fd, &statBuf))
                        {
                            assert(r == -1);
                            throw ErrnoException(errno);
                        }

                        /* The st_size field only makes sense for regular files
                           and symbolic links. We support regular files only: */
                        if ((statBuf.st_mode & S_IFMT) != S_IFREG)
                            throw FileTypeNotSupportedException();

                        assert(statBuf.st_size >= 0);
                        using S = decltype(statBuf.st_size);
                        using U = std::make_unsigned<S>::type;
                        static_assert(
                                std::numeric_limits<U>::max()
                                <= std::numeric_limits<std::size_t>::max(), "");
                        return static_cast<U>(statBuf.st_size);
                    }(inFd);

            if (leftToRead > 0u) {
                contents.resize(leftToRead);

                auto * buf = contents.data();

                constexpr static ::ssize_t const max =
                        std::numeric_limits<::ssize_t>::max();
                static_assert(max > 0, "");

                using U = std::make_unsigned<::ssize_t>::type;
                constexpr static U const umax = static_cast<U>(max);

                for (;;) {
                    auto const toReadAtOnce(std::min(umax, leftToRead));
                    auto const read = ::read(inFd, buf, toReadAtOnce);
                    assert(static_cast<U>(read) <= toReadAtOnce);
                    if (read == 0) {
                        throw FileSizeChangedException();
                    } else if (read < 0) {
                        assert(read == -1);
                        if (errno != EINTR)
                            throw ErrnoException(errno);
                    } else {
                        leftToRead -= static_cast<U>(read);
                        if (leftToRead <= 0u)
                            break;
                        buf = ptrAdd(buf, read);
                    }
                }
            }

            ::close(inFd);
        } catch (...) {
            if (inFd >= 0)
                ::close(inFd);
            std::throw_with_nested(std::move(loadException));
        }

        return contents;
    }

private: /* Fields: */

    Container m_data;

};

inline HashValue hash_value(Datum const & datum) noexcept
{ return datum.hash(); }

} /* namespace sharemind { */

#endif /* SHAREMIND_DATUM */
