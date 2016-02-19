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

#ifndef SHAREMIND_UNALIGNEDREFERENCE_H
#define SHAREMIND_UNALIGNEDREFERENCE_H

#include <cstring>
#include <memory>
#include <type_traits>
#include "CopyCv.h"


namespace sharemind {

template <typename T>
class UnalignedReference {

    // Due to std::memcpy not working on volatile:
    static_assert(
            !std::is_volatile<T>::value,
            "Unaligned references to volatile storage are not yet supported!");

public: /* Types: */

    using type = UnalignedReference<T>;

public: /* Methods: */

    UnalignedReference(CopyCv_t<void, T> * const unalignedData) noexcept
        : m_unalignedData(unalignedData)
    {}

    operator typename std::remove_cv<T>::type () const noexcept {
        typename std::remove_cv<T>::type v;
        /** \todo Implement memcpy for any void volatile and remove the relevant
                  static assertion above. Be sure to also augment the memcpy in
                  operator=. */
        std::memcpy(std::addressof(v), m_unalignedData, sizeof(T));
        return v;
    }

    template <typename T_>
    typename std::enable_if<
        !std::is_const<T>::value && std::is_convertible<T_, T const &>::value,
        type
    >::type &
    operator=(T_ && v) noexcept {
        std::memcpy(this->m_unalignedData, std::addressof(v), sizeof(T));
        return *this;
    }

private: /* Fields: */

    CopyCv_t<void, T> * const m_unalignedData;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_UNALIGNEDREFERENCE_H */
