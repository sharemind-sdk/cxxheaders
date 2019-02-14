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

#ifndef SHAREMIND_STRINGVIEW_H
#define SHAREMIND_STRINGVIEW_H

#include <algorithm>
#include <boost/functional/hash_fwd.hpp>
#include <cstddef>
#include <limits>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include "IntegralComparisons.h"
#include "TypeIdentity.h"


namespace sharemind {
namespace Detail {
namespace StringView {

template <typename CharT, typename Traits, typename SizeType>
inline auto doOut(std::basic_ostream<CharT, Traits> & os,
                  CharT const * data,
                  SizeType size)
        -> std::enable_if_t<
                std::numeric_limits<std::streamsize>::max()
                >= std::numeric_limits<SizeType>::max(),
                void
            >
{
    auto const s = static_cast<std::streamsize>(size);
    auto const r = os.rdbuf()->sputn(data, s);
    if (r != s)
        os.setstate((r <= 0) ? std::ios_base::badbit : std::ios_base::failbit);
}

template <typename CharT, typename Traits, typename SizeType>
inline auto doOut(std::basic_ostream<CharT, Traits> & os,
                  CharT const * data,
                  SizeType size)
        -> std::enable_if_t<
                std::numeric_limits<std::streamsize>::max()
                < std::numeric_limits<SizeType>::max(),
                void
            >
{
    auto errorBit = std::ios_base::failbit;
    static constexpr auto const patchSize =
            std::numeric_limits<std::streamsize>::max();
    for (; size > patchSize; size -= patchSize) {
        auto const r = os.rdbuf()->sputn(data, patchSize);
        if (r != patchSize) {
            os.setstate(errorBit);
            return;
        }
        errorBit = std::ios_base::badbit;
    }
    auto const s = static_cast<std::streamsize>(size);
    if (os.rdbuf()->sputn(data, s) != s)
        os.setstate(errorBit);
}

template <typename CharT, typename Traits>
inline void doFill(std::basic_ostream<CharT, Traits> & os, std::streamsize n) {
    auto const c(os.fill());
    for (std::streamsize i = 0; i < n; ++i) {
        if (Traits::eq_int_type(os.rdbuf()->sputc(c), Traits::eof())) {
            os.setstate((i > 0)
                        ? std::ios_base::badbit
                        : std::ios_base::failbit);
            return;
        }
    }
}

} /* namespace StringView { */
} /* namespace Detail { */

template <typename CharT, typename Traits = std::char_traits<CharT> >
class BasicStringView {

    friend std::basic_ostream<CharT, Traits> & operator<<(
                std::basic_ostream<CharT, Traits> & os,
                BasicStringView const v)
    {
        using Sentry = typename std::basic_ostream<CharT, Traits>::sentry;
        if (auto sentry = Sentry(os)) {
            auto const w(os.width());
            if (integralGreaterEqual(v.m_size, w)) {
                Detail::StringView::doOut(os, v.m_start, v.m_size);
            } else if ((os.flags() & std::ios_base::adjustfield)
                       == std::ios_base::left)
            {
                Detail::StringView::doOut(os, v.m_start, v.m_size);
                if (os.good())
                    Detail::StringView::doFill(os, w - v.m_size);
            } else {
                Detail::StringView::doFill(os, w - v.m_size);
                if (os.good())
                    Detail::StringView::doOut(os, v.m_start, v.m_size);
            }
            os.width(0);
        }
        return os;
    }

public: /* Types: */

    using TraitsType = Traits;
    using ValueType = CharT;
    using Pointer = CharT *;
    using ConstPointer = CharT const *;
    using Reference = CharT &;
    using ConstReference = CharT const &;
    using ConstIterator = CharT const *;
    using Iterator = ConstIterator;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
    using ReverseIterator = ConstReverseIterator;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;

public: /* Constants: */

    static constexpr SizeType const npos = SizeType(-1);

public: /* Methods: */

    constexpr BasicStringView() noexcept = default;

    template <typename Allocator>
    constexpr BasicStringView(
            std::basic_string<CharT, Traits, Allocator> const & str) noexcept
        : m_start(str.c_str())
        , m_size(str.size())
    {}

    constexpr BasicStringView(CharT const * str) noexcept
        : m_start(str)
        , m_size(str ? TraitsType::length(str) : 0u)
    {}

    constexpr BasicStringView(CharT const * str, SizeType size) noexcept
        : m_start(str)
        , m_size(size)
    {}

    constexpr BasicStringView(BasicStringView &&) noexcept = default;
    constexpr BasicStringView(BasicStringView const &) noexcept = default;

    constexpr BasicStringView & operator=(BasicStringView &&) noexcept
            = default;
    constexpr BasicStringView & operator=(BasicStringView const &) noexcept
            = default;

    constexpr ConstIterator begin() const noexcept { return m_start; }
    constexpr ConstIterator cbegin() const noexcept { return m_start; }
    constexpr ConstIterator end() const noexcept { return m_start + m_size; }
    constexpr ConstIterator cend() const noexcept { return m_start + m_size; }

    constexpr ConstReverseIterator rbegin() const noexcept
    { return ConstReverseIterator(end()); }

    constexpr ConstReverseIterator crbegin() const noexcept
    { return ConstReverseIterator(cend()); }

    constexpr ConstReverseIterator rend() const noexcept
    { return ConstReverseIterator(begin()); }

    constexpr ConstReverseIterator crend() const noexcept
    { return ConstReverseIterator(cbegin()); }

    constexpr SizeType length() const noexcept { return m_size; }
    constexpr SizeType size() const noexcept { return m_size; }

    constexpr bool empty() const noexcept { return m_size == 0u; }

    constexpr ConstPointer data() const noexcept { return m_start; }

    constexpr ConstReference front() const noexcept { return *m_start; }
    constexpr ConstReference back() const noexcept
    { return *(m_start + (m_size - 1u)); }

    constexpr ConstReference operator[](SizeType n) const noexcept
    { return *(m_start + n); }

    constexpr ConstReference at(SizeType n) const {
        if (n >= m_size)
            throw std::out_of_range("BasicStringView::at(): pos >= size()!");
        return *(m_start + n);
    }

    constexpr void removePrefix(SizeType n) noexcept {
        //assert(n <= m_size);
        m_start += n;
        m_size -= n;
    }
    constexpr void removeSuffix(SizeType n) noexcept {
        //assert(n <= m_size);
        m_size -= n;
    }

    constexpr void swap(BasicStringView & other) noexcept {
        auto tmp(*this);
        (*this) = other;
        other = tmp;
    }

    constexpr BasicStringView substr(SizeType pos, SizeType n = npos) const {
        if (pos > m_size)
            throw std::out_of_range("BasicStringView::substr(): pos > size()!");
        return BasicStringView(m_start + pos, std::min(n, m_size - pos));
    }

    constexpr BasicStringView left(SizeType n) const {
        if (n > m_size)
            throw std::out_of_range("BasicStringView::left(): n > size()!");
        return BasicStringView(m_start, n);
    }

    constexpr BasicStringView right(SizeType n) const {
        if (n > m_size)
            throw std::out_of_range("BasicStringView::right(): n > size()!");
        return BasicStringView(m_start + (m_size - n), n);
    }

    constexpr BasicStringView leftClipped(SizeType n) const {
        if (n > m_size)
            throw std::out_of_range(
                    "BasicStringView::leftClipped(): n > size()!");
        return BasicStringView(m_start + n, m_size - n);
    }

    constexpr BasicStringView rightClipped(SizeType n) const {
        if (n > m_size)
            throw std::out_of_range(
                    "BasicStringView::rightClipped(): n > size()!");
        return BasicStringView(m_start, m_size - n);
    }

    constexpr BasicStringView clipped(SizeType l, SizeType r) const {
        if ((l > m_size) || (r > m_size - l))
            throw std::out_of_range(
                    "BasicStringView::clipped(): l + r > size()!");
        return BasicStringView(m_start + l, m_size - l - r);
    }

    constexpr int compare(BasicStringView other) const noexcept {
        if (auto r = TraitsType::compare(m_start,
                                         other.m_start,
                                         std::min(m_size, other.m_size)))
            return r;
        if (m_size == other.m_size)
            return 0;
        return (m_size < other.m_size) ? -1 : 1;
    }

    constexpr int compare(SizeType pos, SizeType n, BasicStringView other)
            const noexcept
    { return substr(pos, n).compare(other); }

    constexpr int compare(SizeType pos,
                          SizeType n,
                          BasicStringView other,
                          SizeType pos2,
                          SizeType n2) const noexcept
    { return substr(pos, n).compare(other.substr(pos2, n2)); }

    constexpr int compare(CharT const * str) const noexcept
    { return compare(BasicStringView(str)); }

    constexpr int compare(SizeType pos, SizeType n, CharT const * str)
            const noexcept
    { return substr(pos, n).compare(BasicStringView(str)); }

    constexpr int compare(SizeType pos,
                          SizeType n,
                          CharT const * str,
                          SizeType size) const noexcept
    { return substr(pos, n).compare(BasicStringView(str, size)); }

    constexpr bool startsWith(BasicStringView v) const noexcept
    { return (m_size >= v.m_size) && (compare(0u, v.m_size, v) == 0); }

    constexpr bool startsWith(CharT c) const noexcept
    { return (m_size > 0u) && (Traits::eq(front(), c)); }

    /// \todo Optimize not to iterate over str twice:
    constexpr bool startsWith(CharT const * str) const noexcept
    { return startsWith(BasicStringView(str)); }

    constexpr bool endsWith(BasicStringView v) const noexcept {
        return (m_size >= v.m_size)
                && (compare(m_size - v.m_size, v.m_size, v) == 0);
    }

    constexpr bool endsWith(CharT c) const noexcept
    { return (m_size > 0u) && (Traits::eq(back(), c)); }

    constexpr bool endsWith(CharT const * str) const noexcept
    { return endsWith(BasicStringView(str)); }

    constexpr SizeType find(CharT c, SizeType pos = 0u) const noexcept {
        if (pos < m_size)
            if (auto const * p = Traits::find(m_start + pos, m_size - pos, c))
                return static_cast<SizeType>(p - m_start);
        return npos;
    }

    constexpr SizeType find(CharT const * str, SizeType pos, SizeType count)
            const noexcept
    {
        if (count <= 0)
            return (pos <= m_size) ? pos : npos;
        if (count <= m_size)
            for (; pos <= m_size - count; ++pos)
                if (Traits::compare(m_start + pos, str, count) == 0)
                    return pos;
        return npos;
    }

    constexpr SizeType find(BasicStringView v, SizeType pos = 0u) const noexcept
    { return find(v.m_start, pos, v.m_size); }

    constexpr SizeType find(CharT const * str, SizeType pos = 0u) const noexcept
    { return find(BasicStringView(str), pos); }

    constexpr SizeType rfind(CharT c, SizeType pos = npos) const noexcept {
        // Search [0, pos] U [0, m_size):
        if (auto s = m_size)
            /* Initialize i to be the "previous" search position. Note that
               std::min(s - 1u, pos) is always less than SizeType(-1), therefore
               i is always initialized to be greater than 0: */
            for (auto i = std::min(s - 1u, pos) + 1u; i--;)
                if (Traits::eq(m_start[i], c))
                    return i;
        return npos;
    }

    constexpr SizeType rfind(CharT const * str, SizeType pos, SizeType count)
            const noexcept
    {
        if (count <= m_size) {
            {
                auto const maxFirstPos = m_size - count;
                if (maxFirstPos < pos)
                    pos = maxFirstPos;
            }
            for (;; --pos) {
                if (Traits::compare(m_start + pos, str, count) == 0)
                    return pos;
                if (pos <= 0)
                    break;
            }
        }
        return npos;
    }

    constexpr SizeType rfind(BasicStringView v, SizeType pos = 0u) const noexcept
    { return rfind(v.m_start, pos, v.m_size); }

    constexpr SizeType rfind(CharT const * str, SizeType pos = 0u) const noexcept
    { return rfind(BasicStringView(str), pos); }

    constexpr SizeType findFirstOf(CharT c, SizeType pos = 0u) const noexcept {
        for (; pos < m_size; ++pos)
            if (Traits::eq(m_start[pos], c))
                return pos;
        return npos;
    }

    constexpr SizeType findFirstOf(CharT const * str,
                                   SizeType pos,
                                   SizeType count) const noexcept
    {
        for (; pos < m_size; ++pos)
            if (Traits::find(str, count, m_start[pos]))
                return pos;
        return npos;
    }

    constexpr SizeType findFirstOf(BasicStringView v, SizeType pos = 0u)
            const noexcept
    { return findFirstOf(v.m_start, pos, v.m_size); }

    constexpr SizeType findFirstOf(CharT const * str, SizeType pos = 0u)
            const noexcept
    { return findFirstOf(BasicStringView(str), pos); }

    constexpr SizeType findFirstNotOf(CharT c, SizeType pos = 0u) const noexcept {
        for (; pos < m_size; ++pos)
            if (!Traits::eq(m_start[pos], c))
                return pos;
        return npos;
    }

    constexpr SizeType findFirstNotOf(CharT const * str,
                                      SizeType pos,
                                      SizeType count) const noexcept
    {
        for (; pos < m_size; ++pos)
            if (!Traits::find(str, count, m_start[pos]))
                return pos;
        return npos;
    }

    constexpr SizeType findFirstNotOf(BasicStringView v, SizeType pos = 0u)
            const noexcept
    { return findFirstNotOf(v.m_start, pos, v.m_size); }

    constexpr SizeType findFirstNotOf(CharT const * str, SizeType pos = 0u)
            const noexcept
    { return findFirstNotOf(BasicStringView(str), pos); }

    constexpr SizeType findLastOf(CharT c, SizeType pos = npos) const noexcept {
        // Search [0, pos] U [0, m_size):
        if (auto const s = m_size)
            /* Initialize i to be the "previous" search position. Note that
               std::min(s - 1u, pos) is always less than SizeType(-1), therefore
               i is always initialized to be greater than 0: */
            for (SizeType i = std::min(s - 1u, pos) + 1u; i--;)
                if (Traits::eq(m_start[i], c))
                    return i;
        return npos;
    }

    constexpr SizeType findLastOf(CharT const * str,
                                  SizeType pos,
                                  SizeType count) const noexcept
    {
        // Search [0, pos] U [0, m_size):
        if (auto const s = m_size)
            /* Initialize i to be the "previous" search position. Note that
               std::min(s - 1u, pos) is always less than SizeType(-1), therefore
               i is always initialized to be greater than 0: */
            for (SizeType i = std::min(s - 1u, pos) + 1u; i--;)
                if (Traits::find(str, count, m_start[i]))
                    return i;
        return npos;
    }

    constexpr SizeType findLastOf(BasicStringView v, SizeType pos = npos)
            const noexcept
    { return findLastOf(v.m_start, pos, v.m_size); }

    constexpr SizeType findLastOf(CharT const * str, SizeType pos = npos)
            const noexcept
    { return findLastOf(BasicStringView(str), pos); }

    constexpr SizeType findLastNotOf(CharT c, SizeType pos = npos) const noexcept {
        // Search [0, pos] U [0, m_size):
        if (auto const s = m_size)
            /* Initialize i to be the "previous" search position. Note that
               std::min(s - 1u, pos) is always less than SizeType(-1), therefore
               i is always initialized to be greater than 0: */
            for (SizeType i = std::min(s - 1u, pos) + 1u; i--;)
                if (!Traits::eq(m_start[i], c))
                    return i;
        return npos;
    }

    constexpr SizeType findLastNotOf(CharT const * str,
                                     SizeType pos,
                                     SizeType count) const noexcept {
        // Search [0, pos] U [0, m_size):
        if (auto const s = m_size)
            /* Initialize i to be the "previous" search position. Note that
               std::min(s - 1u, pos) is always less than SizeType(-1), therefore
               i is always initialized to be greater than 0: */
            for (SizeType i = std::min(s - 1u, pos) + 1u; i--;)
                if (!Traits::find(str, count, m_start[i]))
                    return i;
        return npos;
    }

    constexpr SizeType findLastNotOf(BasicStringView v, SizeType pos = npos)
            const noexcept
    { return findLastNotOf(v.m_start, pos, v.m_size); }

    constexpr SizeType findLastNotOf(CharT const * str, SizeType pos = npos)
            const noexcept
    { return findLastNotOf(BasicStringView(str), pos); }

    constexpr SizeType copy(CharT * dest, SizeType count, SizeType pos = 0u)
            const
    {
        if (pos > m_size)
            throw std::out_of_range("BasicStringView::copy(): pos > size()!");
        auto toCopy(std::min(count, m_size - pos));
        Traits::copy(dest, m_start + pos, toCopy);
        return toCopy;
    }

    std::basic_string<CharT, Traits> str() const
    { return std::basic_string<CharT, Traits>(m_start, m_size); }

    template <typename Allocator>
    auto str(Allocator const & alloc = Allocator()) const
            -> std::basic_string<CharT, Traits, Allocator>
    {
        return std::basic_string<CharT, Traits, Allocator>(m_start,
                                                           m_size,
                                                           alloc);
    }

    /** \todo Implement max_size(), if at all possible. */

private: /* Fields: */

    ConstIterator m_start = nullptr;
    SizeType m_size = 0u;

};

#define SHAREMIND_STRINGVIEW_H_COMP(op) \
    template <typename CharT, typename Traits> \
    constexpr bool operator op(BasicStringView<CharT, Traits> lhs, \
                               BasicStringView<CharT, Traits> rhs) noexcept \
    { return lhs.compare(rhs) op 0; } \
    template <typename CharT, typename Traits> \
    constexpr bool operator op( \
            BasicStringView<CharT, Traits> lhs, \
            TypeIdentityT<BasicStringView<CharT, Traits> > rhs) noexcept \
    { return lhs.compare(rhs) op 0; } \
    template <typename CharT, typename Traits> \
    constexpr bool operator op( \
            TypeIdentityT<BasicStringView<CharT, Traits> > lhs, \
            BasicStringView<CharT, Traits> rhs) noexcept \
    { return lhs.compare(rhs) op 0; }
SHAREMIND_STRINGVIEW_H_COMP(==)
SHAREMIND_STRINGVIEW_H_COMP(!=)
SHAREMIND_STRINGVIEW_H_COMP(<)
SHAREMIND_STRINGVIEW_H_COMP(<=)
SHAREMIND_STRINGVIEW_H_COMP(>=)
SHAREMIND_STRINGVIEW_H_COMP(>)
#undef SHAREMIND_STRINGVIEW_H_COMP

#define SHAREMIND_STRINGVIEW_H_SPEC(ClassName, CharT) \
    using ClassName = BasicStringView<CharT>; \
    namespace StringViewLiterals { \
    constexpr ClassName operator""_sv(CharT const * str, std::size_t len) \
            noexcept \
    { return ClassName(str, len); } \
    } /* namespace StringViewLiterals { */
SHAREMIND_STRINGVIEW_H_SPEC(StringView,char)
SHAREMIND_STRINGVIEW_H_SPEC(WideStringView,wchar_t)
/** \todo SHAREMIND_STRINGVIEW_H_SPEC(U8StringView,char8_t) // for C++20 */
SHAREMIND_STRINGVIEW_H_SPEC(U16StringView,char16_t)
SHAREMIND_STRINGVIEW_H_SPEC(U32StringView,char32_t)
#undef SHAREMIND_STRINGVIEW_H_SPEC

/// \todo Check if the following comment holds once C++20 is out:
/** \note We specialize only for boost::hash and not std::hash, because the
          latter hasher would need to be identical to hashes of
          std::basic_string objects with the same contents, but since the C++
          standard library itself does not provide access to the underlying hash
          function, there is no portable way to ensure that. */
template <typename CharT, typename Traits>
inline std::size_t hash_value(BasicStringView<CharT, Traits> const & v)
        noexcept(noexcept(boost::hash_range(v.begin(), v.end())))
{ return boost::hash_range(v.begin(), v.end()); }

} /* namespace Sharemind { */

#endif /* SHAREMIND_STRINGVIEW_H */
