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

#ifndef SHAREMIND_DETAIL_UNALIGNEDREFERENCEBASE_H
#define SHAREMIND_DETAIL_UNALIGNEDREFERENCEBASE_H

#include <cstring>
#include <type_traits>
#include "../CopyCv.h"


namespace sharemind {

template <typename T>
class UnalignedReferenceBase {

public: /* Types: */

    using StorageType = CopyCv_t<void, T>;

public: /* Methods: */

    UnalignedReferenceBase(StorageType * const unalignedData) noexcept
        : m_unalignedData(unalignedData)
    {}

    StorageType * ptr() const noexcept { return m_unalignedData; }

    void read(typename std::remove_cv<T>::type & v) const noexcept
    { std::memcpy(&v, m_unalignedData, sizeof(T)); }

    typename std::remove_cv<T>::type get() const noexcept {
        typename std::remove_cv<T>::type v;
        read(v);
        return v;
    }

    T operator*() const noexcept { return get(); }

private: /* Fields: */

    StorageType * m_unalignedData;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_DETAIL_UNALIGNEDREFERENCEBASE_H */
