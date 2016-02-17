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

#ifndef SHAREMIND_PACKEDSTRUCTACCESSOR_H
#define SHAREMIND_PACKEDSTRUCTACCESSOR_H

#include "PackingInfo.h"


namespace sharemind {

template <typename ... Ts>
class PackedStructAccessor {

public: /* Constants: */

    SHAREMIND_PACKINGINFO_DECLARE_MEMBER_CONSTANTS(Ts...)

public: /* Types: */

    using type = PackedStructAccessor<Ts...>;

    SHAREMIND_PACKINGINFO_DECLARE_MEMBER_TYPES(Ts...)

public: /* Methods: */

    PackedStructAccessor(void * const data) noexcept : m_data(data) {}

    PackedStructAccessor(type &&) noexcept = default;
    PackedStructAccessor(type const &) noexcept = default;

    type & operator=(type &&) noexcept = default;
    type & operator=(type const &) noexcept = default;

    SHAREMIND_PACKINGINFO_DEFINE_READ_METHODS(Ts...)
    SHAREMIND_PACKINGINFO_DEFINE_WRITE_METHODS(const,Ts...)

    void * data() const noexcept { return m_data; }

private: /* Fields: */

    void * m_data;

}; /* class PackedStructAccessor */

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKEDSTRUCTACCESSOR_H */
