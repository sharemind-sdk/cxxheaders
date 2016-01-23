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

template <typename> class UnalignedReference;

namespace Detail {

template <typename T>
class UnalignedReferenceBase {

    // Due to std::memcpy not working on volatile:
    static_assert(
            !std::is_volatile<T>::value,
            "Unaligned references to volatile storage are not yet supported!");

    template <typename> friend class sharemind::UnalignedReference;

public: /* Types: */

    using StorageType = CopyCv_t<void, T>;

public: /* Methods: */

    UnalignedReferenceBase(StorageType * const unalignedData) noexcept
        : m_unalignedData(unalignedData)
    {}

    operator typename std::remove_cv<T>::type () const noexcept {
        typename std::remove_cv<T>::type v;
        /** \todo Implement memcpy for any void volatile and remove the relevant
                  static assertion above. Be sure to also augment the memcpy in
                  UnaligneReference<T>::operator=. */
        std::memcpy(&v, m_unalignedData, sizeof(T));
        return v;
    }

private: /* Fields: */

    StorageType * const m_unalignedData;

};

} /* namespace Detail { */
} /* namespace sharemind { */

#endif /* SHAREMIND_DETAIL_UNALIGNEDREFERENCEBASE_H */
