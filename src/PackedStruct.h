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

#ifndef SHAREMIND_PACKEDSTRUCT_H
#define SHAREMIND_PACKEDSTRUCT_H

#include <cstddef>
#include <cstdint>
#include "ConstPackedStructAccessor.h"
#include "PackedStructAccessor.h"
#include "PackedRawData.h"
#include "PackingInfo.h"
#include "TemplateInstantiateWithTypeParams.h"
#include "TemplatePrefixTypes.h"
#include "TemplateSuffixTypes.h"


namespace sharemind {

template <typename ... Ts>
class __attribute__((packed)) PackedStruct {

public: /* Types: */

    using type = PackedStruct<Ts...>;

    SHAREMIND_PACKINGINFO_DECLARE_MEMBER_TYPES(Ts...)

    template <std::size_t I>
    using PrefixType =
            TemplateInstantiateWithTypeParams_t<
                PackedStruct,
                TemplatePrefixTypes_t<I, Ts...>
            >;

    template <std::size_t I>
    using SuffixType =
            TemplateInstantiateWithTypeParams_t<
                PackedStruct,
                TemplateSuffixTypes_t<I, Ts...>
            >;

    using StaticPrefixType = type;

public: /* Methods: */

    PackedStructAccessor<Ts...> refs() noexcept { return data(); }
    ConstPackedStructAccessor<Ts...> refs() const noexcept { return data(); }
    ConstPackedStructAccessor<Ts...> crefs() const noexcept { return data(); }

    SHAREMIND_PACKINGINFO_DEFINE_READ_METHODS(Ts...)
    SHAREMIND_PACKINGINFO_DEFINE_WRITE_METHODS(,Ts...)

    void * data() noexcept { return &m_data; }
    void const * data() const noexcept { return &m_data; }
    void const * constData() const noexcept { return &m_data; }

    uint8_t * dataUint8() noexcept { return m_data.asUint8; }
    uint8_t const * dataUint8() const noexcept { return m_data.asUint8; }
    uint8_t const * constDataUint8() const noexcept { return m_data.asUint8; }

    char * dataChar() noexcept { return m_data.asChar; }
    char const * dataChar() const noexcept { return m_data.asChar; }
    char const * constDataChar() const noexcept { return m_data.asChar; }

    unsigned char * dataUnsignedChar() noexcept { return m_data.asUChar; }
    unsigned char const * dataUnsignedChar() const noexcept
    { return m_data.asUChar; }
    unsigned char const * constDataUnsignedChar() const noexcept
    { return m_data.asUChar; }

private: /* Fields: */

    PackedRawData<type::staticSize()> m_data;

}; /* class PackedStruct */

template <typename T> struct PackedStructFromTemplate {};

template <template <typename...> class Tmpl, typename ... Ts>
struct PackedStructFromTemplate<Tmpl<Ts...> >
{ using type = PackedStruct<Ts...>; };

template <typename T>
using PackedStructFromTemplate_t = typename PackedStructFromTemplate<T>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_PACKEDSTRUCT_H */
