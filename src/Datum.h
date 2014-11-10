/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_DATUM
#define SHAREMIND_DATUM

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/static_assert.hpp>
#include <fstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace sharemind {

class Datum {

private: /* Types: */

    typedef std::vector<char> Container;

public: /* Methods: */

    inline Datum()
            noexcept(std::is_nothrow_default_constructible<Container>::value)
    {}

    inline Datum(Datum && move)
            noexcept(std::is_nothrow_move_constructible<Container>::value)
        : m_data(std::move(move.m_data)) {}

    inline Datum(const Datum & copy) : m_data(copy.m_data) {}

    inline Datum(Container::size_type size,
                 Container::value_type initial = Container::value_type())
        : m_data(size, initial) {}

    inline Datum(const void * const data, const size_t size)
        : m_data(static_cast<Container::value_type const *>(data),
                 static_cast<Container::value_type const *>(data) + size) {}

    inline Datum(const std::string & filename)
    { loadFileToVector(m_data, filename); }

    inline Datum & operator=(const Datum & copy) {
        m_data = copy.m_data;
        return *this;
    }

    inline Datum & operator=(Datum && move)
            noexcept(std::is_nothrow_move_assignable<Container>::value)
    {
        m_data = std::move(move.m_data);
        return *this;
    }

    inline bool operator==(const Datum & rhs) const noexcept
    { return m_data == rhs.m_data; }

    inline bool operator!=(const Datum & rhs) const noexcept
    { return m_data != rhs.m_data; }

    inline void resize(const size_t size,
                       Container::value_type initial = Container::value_type())
    { m_data.resize(size, initial); }

    inline void loadFromFile(const std::string & filename) {
        std::vector<char> newData;
        loadFileToVector(newData, filename);
        m_data = newData;
    }

    inline void assign(const void * const data, const size_t size) {
        m_data.resize(size);
        std::copy(static_cast<const char *>(data),
                  static_cast<const char *>(data) + size,
                  &m_data[0]);
    }

    inline void clear() noexcept { m_data.clear(); }
    inline bool empty() const noexcept { return m_data.empty(); }
    inline Container::size_type size() const noexcept { return m_data.size(); }
    inline void * data() noexcept { return &m_data[0u]; }
    inline void * dataEnd() noexcept { return &*m_data.end(); }
    inline const void * constData() const noexcept { return &m_data[0u]; }
    inline const void * constDataEnd() const noexcept
    { return &*m_data.end(); }

    template <class T>
    inline static void loadFileToVector(std::vector<T> & outData,
                                        const std::string & filename)
    {
        std::ifstream inFile(filename.c_str(),
                             std::ios_base::in | std::ios_base::binary);
        inFile.seekg(0, std::ios::end);
        const std::streamoff fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);
        outData.resize(fileSize);
        inFile.read(static_cast<char *>(&outData[0]), fileSize);
    }

    static std::size_t hashData(const void * data, const size_t size) noexcept {
        const char * dataStart = static_cast<const char *>(data);
        std::size_t hash = size;
        boost::hash_range(hash, dataStart, dataStart + size);
        return hash;
    }

private: /* Fields: */

    Container m_data;

};

inline std::size_t hash_value(const Datum & datum) noexcept
{ return Datum::hashData(datum.constData(), datum.size()); }

} /* namespace sharemind { */

#endif /* SHAREMIND_DATUM */
