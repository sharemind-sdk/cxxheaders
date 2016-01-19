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

#ifndef SHAREMIND_UUID_H
#define SHAREMIND_UUID_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/name_generator.hpp>
#include <utility>


namespace sharemind {

using Uuid = boost::uuids::uuid;
static_assert(sizeof(Uuid) == 16u, "");

using UuidNameGenerator = boost::uuids::name_generator;

/* Level 0: */
constexpr Uuid sharemindNamespaceUuid =
        {{0x53u, 0x68u, 0x61u, 0x72u, 0x65u, 0x6du, 0x69u, 0x6eu,
          0x64u, 0x7eu, 0x28u, 0x5eu, 0x5fu, 0x5eu, 0x29u, 0x7eu}};


/* Level 1: */
class SharemindUuidGenerator: public UuidNameGenerator {

public: /* Methods: */

    inline SharemindUuidGenerator() noexcept
        : UuidNameGenerator{sharemindNamespaceUuid}
    {}

};

template <typename ... Args>
inline Uuid generateSharemindNameUuid(Args && ... args) noexcept {
    static SharemindUuidGenerator generator;
    return generator(std::forward<Args>(args)...);
}


} /* namespace sharemind { */

#endif /* SHAREMIND_UUID_H */