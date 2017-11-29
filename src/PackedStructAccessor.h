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

#ifndef SHAREMIND_PACKEDSTRUCTACCESSOR_H
#define SHAREMIND_PACKEDSTRUCTACCESSOR_H

#include "PackingInfo.h"

#include <cstddef>
#include "compiler-support/ClangPR26692.h"
#include "TemplateInstantiateWithTypeParams.h"
#include "TemplatePrefixTypes.h"
#include "TemplateSuffixTypes.h"


namespace sharemind {

template <typename ... Ts>
class PackedStructAccessor {

public: /* Types: */

    using type = PackedStructAccessor<Ts...>;

    SHAREMIND_PACKINGINFO_DECLARE_MEMBER_TYPES(Ts...)

    template <std::size_t I>
    using PrefixType =
            TemplateInstantiateWithTypeParams_t<
                SHAREMIND_CLANGPR26692_WORKAROUND(sharemind)
                    PackedStructAccessor,
                TemplatePrefixTypes_t<I, Ts...>
            >;

    template <std::size_t I>
    using SuffixType =
            TemplateInstantiateWithTypeParams_t<
                SHAREMIND_CLANGPR26692_WORKAROUND(sharemind)
                    PackedStructAccessor,
                TemplateSuffixTypes_t<I, Ts...>
            >;

    using StaticPrefixType = type;

public: /* Methods: */

    PackedStructAccessor(void * const data) noexcept : m_data(data) {}

    PackedStructAccessor(type &&) noexcept = default;
    PackedStructAccessor(type const &) noexcept = default;

    type & operator=(type &&) noexcept = default;
    type & operator=(type const &) noexcept = default;

    SHAREMIND_PACKINGINFO_DEFINE_READ_METHODS(Ts...)
    SHAREMIND_PACKINGINFO_DEFINE_WRITE_METHODS(const,Ts...)

    void * data() const noexcept { return m_data; }
    void const * constData() const noexcept { return m_data; }

private: /* Fields: */

    void * m_data;

}; /* class PackedStructAccessor */

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKEDSTRUCTACCESSOR_H */
