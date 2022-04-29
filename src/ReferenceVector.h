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

#ifndef SHAREMIND_REFERENCEVECTOR_H
#define SHAREMIND_REFERENCEVECTOR_H

#include <cstddef>
#include <iterator>
#include <type_traits>
#include "AssertReturn.h"
#include "BitVector.h"


namespace sharemind {
namespace Detail {

template <typename T>
class RefVecBase {

public: /* Types: */

    using value_type = T;
    using const_iterator = value_type const *;

public: /* Methods: */

    RefVecBase() noexcept = default;

    RefVecBase(T * const begin, std::size_t size) noexcept
        : m_begin(assertReturn(begin))
        , m_end(begin + size)
    { }

    std::size_t size () const {
        auto r(std::distance(m_begin, m_end));
        using U = typename std::make_unsigned<decltype(r)>::type;
        return static_cast<U>(r);
    }

    value_type const & operator[](std::size_t i) const
    { return *(m_begin + i); }

    bool empty() const { return m_begin >= m_end; }
    const_iterator begin() const { return m_begin; }
    const_iterator end() const { return m_end; }

protected: /* Fields: */

    T * const m_begin = nullptr;
    T * const m_end = nullptr;

}; /* class RefVecBase { */
} /* namespace Detail { */

template <typename T>
class RefVec: Detail::RefVecBase<T> {

private: /* Types: */

    using Base = Detail::RefVecBase<T>;

public: /* Types: */

    using value_type = typename Base::value_type;
    using const_iterator = typename Base::const_iterator;
    using iterator = value_type*;

public: /* Methods: */

    template <typename Reference>
    explicit RefVec(Reference const & ref)
        : Base(static_cast<value_type *>(ref.pData),
               ref.size / sizeof(value_type))
    {}

    iterator begin() { return this->m_begin; }
    iterator end() { return this->m_end; }
    value_type & operator[](std::size_t i) { return *(this->m_begin + i); }

    template <typename Rng>
    void randomize(Rng & rng) {
        if (!empty())
            rng.fillBlock(begin(), end());
    }

    using Base::empty;
    using Base::size;
    using Base::operator [];
    using Base::begin;
    using Base::end;

}; /* class RefVec { */

template <> template <typename Rng>
void RefVec<bool>::randomize(Rng & rng) {
    if (!empty()) {
        BitVec<> bits(size());
        bits.randomize(rng);
        std::size_t i = 0;
        for (auto & element : *this) {
            element = bits[i];
            ++i;
        }
    }
}

template <typename T>
class CRefVec: Detail::RefVecBase<T const> {

private: /* Types: */

    using Base = Detail::RefVecBase<T const>;

public: /* Types: */

    using value_type = typename Base::value_type;
    using const_iterator = typename Base::const_iterator;

public: /* Methods: */

    template <typename ConstReference>
    explicit CRefVec(ConstReference const & cref)
        : Base(static_cast<value_type *>(cref.pData),
               cref.size / sizeof (value_type))
    { }

    explicit CRefVec(RefVec<T> const & immvec)
        : Base(immvec)
    {}

    using Base::empty;
    using Base::size;
    using Base::operator[];
    using Base::begin;
    using Base::end;

}; /* class CRefVec { */

} /* namespace sharemind */

#endif /* SHAREMIND_REFERENCEVECTOR_H */
