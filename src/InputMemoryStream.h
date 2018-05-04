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

#ifndef SHAREMIND_INPUTMEMORYSTREAM_H
#define SHAREMIND_INPUTMEMORYSTREAM_H

#include <cstddef>
#include <istream>
#include <streambuf>
#include <string>


namespace sharemind {
namespace Detail {

template <typename CharT, typename Traits = std::char_traits<CharT> >
struct BasicMemoryBuffer: std::basic_streambuf<CharT, Traits> {

    BasicMemoryBuffer(CharT const * data, std::size_t size) {
        auto d = const_cast<CharT *>(data);
        this->setg(d, d, d + size);
    }

};

} /* namespace Detail { */

template <typename CharT, typename Traits = std::char_traits<CharT> >
class BasicInputMemoryStream
    : private virtual Detail::BasicMemoryBuffer<CharT, Traits>
    , public std::basic_istream<CharT, Traits>
{

public: /* Methods: */

    BasicInputMemoryStream(CharT const * data, std::size_t size)
        : Detail::BasicMemoryBuffer<CharT, Traits>(data, size)
        , std::istream(
              static_cast<Detail::BasicMemoryBuffer<CharT, Traits> *>(this))
    {}

};

using InputMemoryStream = BasicInputMemoryStream<char>;

} /* namespace Sharemind { */

#endif /* SHAREMIND_INPUTMEMORYSTREAM_H */
