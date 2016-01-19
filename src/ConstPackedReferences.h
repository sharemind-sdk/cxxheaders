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

#ifndef SHAREMIND_CONSTPACKEDREFERENCES_H
#define SHAREMIND_CONSTPACKEDREFERENCES_H

#include "PackedReferencesInfo.h"

#include "PackedReferences.h"


namespace sharemind {

template <typename ... Ts>
class __attribute__((packed)) ConstPackedReferences {

    SHAREMIND_PACKEDREFERENCESINFO_DECLARE_MEMBER_TYPES(Ts...)

public: /* Types: */

    using type = ConstPackedReferences<Ts...>;

public: /* Methods: */

    ConstPackedReferences(void const * const data) noexcept : m_data(data) {}

    ConstPackedReferences(PackedReferences<Ts...> const & refs)
        : m_data(refs.data())
    {}

    SHAREMIND_PACKEDREFERENCESINFO_DEFINE_READ_METHODS

    void const * data() const noexcept { return m_data; }

private: /* Fields: */

    void const * const m_data;

}; /* class PackedReferences */

} /* namespace sharemind { */

#endif /* SHAREMIND_CONSTPACKEDREFERENCES_H */
