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

#ifndef SHAREMIND_CONSTPACKEDSTRUCTACCESSOR_H
#define SHAREMIND_CONSTPACKEDSTRUCTACCESSOR_H

#include "PackingInfo.h"

#include "PackedStructAccessor.h"


namespace sharemind {

template <typename ... Ts>
class __attribute__((packed)) ConstPackedStructAccessor {

public: /* Types: */

    using type = ConstPackedStructAccessor<Ts...>;

    SHAREMIND_PACKINGINFO_DECLARE_MEMBER_TYPES(Ts...)

public: /* Methods: */

    ConstPackedStructAccessor(void const * const data) noexcept
        : m_data(data)
    {}

    ConstPackedStructAccessor(PackedStructAccessor<Ts...> const & refs)
        : m_data(refs.data())
    {}

    ConstPackedStructAccessor(type &&) noexcept = default;
    ConstPackedStructAccessor(type const &) noexcept = default;

    type & operator=(type &&) noexcept = default;
    type & operator=(type const &) noexcept = default;

    SHAREMIND_PACKINGINFO_DEFINE_READ_METHODS(Ts...)

    void const * data() const noexcept { return m_data; }

private: /* Fields: */

    void const * m_data;

}; /* class ConstPackedStructAccessor */

} /* namespace sharemind { */

#endif /* SHAREMIND_CONSTPACKEDSTRUCTACCESSOR_H */
