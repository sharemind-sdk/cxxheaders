/*
 * Copyright (C) 2017 Cybernetica
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

#ifndef SHAREMIND_UNINITIALIZEDDYNAMICARRAY_H
#define SHAREMIND_UNINITIALIZEDDYNAMICARRAY_H

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include "MakeUnique.h"


namespace sharemind {

template <typename T>
class UninitializedDynamicArray {

private: /* Types: */

    using AllocType =
            typename std::aligned_storage<sizeof(T), alignof(T)>::type;

public: /* Types: */

    using SizeType = std::size_t;
    using Reference = T &;
    using ConstReference = T const &;
    using Iterator = T *;
    using ConstIterator = T const *;
    using Pointer = T *;
    using ConstPointer = T const *;

public: /* Methods: */

    UninitializedDynamicArray() noexcept {}

    UninitializedDynamicArray(UninitializedDynamicArray && move) noexcept
        : m_size(move.m_size)
        , m_data(std::move(move.m_data))
    { move.m_size = 0u; }

    UninitializedDynamicArray(UninitializedDynamicArray const &) = delete;

    UninitializedDynamicArray(SizeType const size)
        : m_size(size)
        , m_data(makeUnique<AllocType[]>(size))
    {}

    virtual ~UninitializedDynamicArray() noexcept {}

    UninitializedDynamicArray & operator=(
                UninitializedDynamicArray && move) noexcept
    {
        m_size = move.m_size;
        m_data = std::move(move.m_data);
        move.m_size = 0u;
    }

    UninitializedDynamicArray & operator=(
                UninitializedDynamicArray const &) = delete;

    Iterator begin() noexcept { return reinterpret_cast<T *>(m_data.get()); }

    ConstIterator begin() const noexcept
    { return reinterpret_cast<T const *>(m_data.get()); }

    ConstIterator cbegin() const noexcept
    { return reinterpret_cast<T const *>(m_data.get()); }

    Iterator end() noexcept
    { return reinterpret_cast<T *>(m_data.get() + m_size); }

    ConstIterator end() const noexcept
    { return reinterpret_cast<T const *>(m_data.get() + m_size); }

    ConstIterator cend() const noexcept
    { return reinterpret_cast<T const *>(m_data.get() + m_size); }

    Reference operator[](SizeType const index) noexcept
    { return *reinterpret_cast<T *>(m_data.get() + index); }

    ConstReference operator[](SizeType const index) const noexcept
    { return *reinterpret_cast<T const *>(m_data.get() + index); }

    Pointer data() noexcept { return reinterpret_cast<T *>(m_data.get()); }

    ConstPointer data() const noexcept
    { return reinterpret_cast<T const *>(m_data.get()); }

    Pointer data(SizeType const index) noexcept
    { return reinterpret_cast<T *>(m_data.get() + index); }

    ConstPointer data(SizeType const index) const noexcept
    { return reinterpret_cast<T const *>(m_data.get() + index); }

    bool empty() const noexcept { return !m_size; }
    SizeType size() const noexcept { return m_size; }

private: /* Types: */

    SizeType m_size = 0u;
    std::unique_ptr<AllocType[]> m_data;

}; /* template <typename T> class UninitializedDynamicArray */

} /* namespace Sharemind { */

#endif /* SHAREMIND_UNINITIALIZEDDYNAMICARRAY_H */
