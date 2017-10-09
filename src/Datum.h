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

#ifndef SHAREMIND_DATUM
#define SHAREMIND_DATUM

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <errno.h>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "Concat.h"
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

    SHAREMIND_DEFINE_EXCEPTION(sharemind::Exception, Exception);
    SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING(Exception, LoadException);
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                         FileSizeChanged,
                                         "File size changed!");

public: /* Methods: */

    inline Datum()
            noexcept(std::is_nothrow_default_constructible<Container>::value)
    {}

    inline Datum(Datum && move)
            noexcept(std::is_nothrow_move_constructible<Container>::value)
        : m_data(std::move(move.m_data)) {}

    inline Datum(Datum const & copy) : m_data(copy.m_data) {}

    inline explicit Datum(size_type size, value_type initial = value_type())
        : m_data(size, initial)
    {}

    inline Datum(void const * const data, size_type const size)
        : m_data(static_cast<value_type const *>(data),
                 static_cast<value_type const *>(data) + size)
    {}

    inline explicit Datum(std::string const & filename)
        : m_data(loadFileToContainer(filename))
    {}

    inline Datum & operator=(Datum const & copy) {
        m_data = copy.m_data;
        return *this;
    }

    inline Datum & operator=(Datum && move)
            noexcept(std::is_nothrow_move_assignable<Container>::value)
    {
        m_data = std::move(move.m_data);
        return *this;
    }

    inline bool operator==(Datum const & rhs) const noexcept
    { return m_data == rhs.m_data; }

    inline bool operator!=(Datum const & rhs) const noexcept
    { return m_data != rhs.m_data; }

    inline void resize(size_type const size, value_type initial = value_type())
    { m_data.resize(size, initial); }

    inline void loadFromFile(std::string const & filename)
    { m_data = loadFileToContainer(filename); }

    inline void assign(void const * const data, size_type const size) {
        m_data.resize(size);
        std::copy(static_cast<char const *>(data),
                  static_cast<char const *>(data) + size,
                  &m_data[0]);
    }

    inline Hash hash() const noexcept
    { return hashRawData(&m_data[0u], m_data.size()); }

    inline void clear() noexcept { m_data.clear(); }
    inline bool empty() const noexcept { return m_data.empty(); }
    inline size_type size() const noexcept { return m_data.size(); }
    inline void * data() noexcept { return &m_data[0u]; }
    inline void * dataEnd() noexcept { return &*m_data.end(); }
    inline void const * constData() const noexcept { return &m_data[0u]; }
    inline void const * constDataEnd() const noexcept
    { return &*m_data.end(); }

private: /* Methods: */

    inline static Container loadFileToContainer(std::string const & filename) {
        LoadException loadException(
                    concat("Failed to load file \"", filename, "\"!"));
        Container contents;
        long fileSize;
        std::FILE * inFile = nullptr;

        try {
            inFile = std::fopen(filename.c_str(), "rb");
            if (!inFile)
                throw ErrnoException(errno);

            if (std::fseek(inFile, 0, SEEK_END))
                throw ErrnoException(errno);

            fileSize = std::ftell(inFile);
            if (fileSize == -1L)
                throw ErrnoException(errno);

            if (std::fseek(inFile, 0, SEEK_SET))
                throw ErrnoException(errno);

            assert(fileSize >= 0);
            if (fileSize > 0) {
                contents.resize(static_cast<size_type>(fileSize));
            }

            std::size_t pos = 0u;

            do {
                auto read = std::fread(ptrAdd(contents.data(), pos),
                        sizeof(char),
                        contents.size() - pos,
                        inFile);

                if (std::ferror(inFile))
                    throw ErrnoException(errno);

                pos += read;

            } while (pos < fileSize && !std::feof(inFile));

            if (pos != fileSize)
                throw FileSizeChanged();

            std::fclose(inFile);
        } catch (...) {
            if (inFile)
                std::fclose(inFile);
            std::throw_with_nested(std::move(loadException));
        }

        return contents;
    }

private: /* Fields: */

    Container m_data;

};

inline Hash hash_value(Datum const & datum) noexcept { return datum.hash(); }

} /* namespace sharemind { */

#endif /* SHAREMIND_DATUM */
