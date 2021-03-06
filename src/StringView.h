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

template <typename CharT, typename Traits, typename SizeType>
inline void doFill(std::basic_ostream<CharT, Traits> & os, SizeType n) {
    static_assert(std::is_unsigned<SizeType>::value, "");
    auto const c(os.fill());
    for (SizeType i = 0u; i < n; ++i) {
        if (Traits::eq_int_type(os.rdbuf()->sputc(c), Traits::eof())) {
            os.setstate(i ? std::ios_base::badbit : std::ios_base::failbit);
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
            if (w == 0) {
                Detail::StringView::doOut(os, v.m_start, v.m_size);
            } else {
                if (w > 0) {
                    if (integralGreaterEqual(v.m_size, w)) {
                        Detail::StringView::doOut(os, v.m_start, v.m_size);
                    } else {
                        using UW =
                                typename std::make_unsigned<decltype(w)>::type;
                        auto const toFill = static_cast<UW>(w) - v.m_size;
                        if ((os.flags() & std::ios_base::adjustfield)
                               == std::ios_base::left)
                        {
                            Detail::StringView::doOut(os, v.m_start, v.m_size);
                            if (os.good())
                                Detail::StringView::doFill(os, toFill);
                        } else {
                            Detail::StringView::doFill(os, toFill);
                            if (os.good())
                                Detail::StringView::doOut(os,
                                                          v.m_start,
                                                          v.m_size);
                        }

                    }
                }
                os.width(0);
            }
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

    /**
      \note This constructor is provided, because we need a means to initialize
            string views from standard string types, but we can't add amend the
            standard string themselves with conversion operators to string
            views.
    */
    template <typename Allocator>
    constexpr BasicStringView(
            std::basic_string<CharT, Traits, Allocator> const & str) noexcept
        : m_start(str.c_str())
        , m_size(str.size())
    {}

    constexpr BasicStringView(CharT const * str) noexcept
        : m_start(str)
        , m_size(str ? Traits::length(str) : 0u)
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

    constexpr int compare(CharT const * str, SizeType n) const noexcept {
        if (auto r = Traits::compare(m_start, str, std::min(m_size, n)))
            return r;
        if (m_size == n)
            return 0;
        return (m_size < n) ? -1 : 1;
    }

    constexpr int compare(BasicStringView other) const noexcept
    { return compare(other.m_start, other.m_size); }

    constexpr int compare(SizeType pos, SizeType n, BasicStringView other) const
    {
        if (pos > m_size)
            throw std::out_of_range(
                    "BasicStringView::compare(): pos > size()!");
        return BasicStringView(m_start + pos,
                               std::min(n, m_size - pos)).compare(other);
    }

    constexpr int compare(SizeType pos,
                          SizeType n,
                          BasicStringView other,
                          SizeType pos2,
                          SizeType n2) const
    {
        if (pos > m_size)
            throw std::out_of_range(
                    "BasicStringView::compare(): pos > size()!");
        if (pos2 > other.m_size)
            throw std::out_of_range(
                    "BasicStringView::compare(): pos2 > other.size()!");
        return BasicStringView(m_start + pos, std::min(n, m_size - pos))
                .compare(other.m_start + pos2,
                         std::min(n2, other.m_size - pos2));
    }

    constexpr int compare(CharT const * str) const noexcept
    { return compare(str, Traits::length(str)); }

    constexpr int compare(SizeType pos, SizeType n, CharT const * str) const {
        if (pos > m_size)
            throw std::out_of_range(
                    "BasicStringView::compare(): pos > size()!");
        return BasicStringView(
                    m_start + pos,
                    std::min(n, m_size - pos)).compare(str,
                                                       Traits::length(str));
    }

    constexpr int compare(SizeType pos,
                          SizeType n,
                          CharT const * str,
                          SizeType size) const
    {
        if (pos > m_size)
            throw std::out_of_range(
                    "BasicStringView::compare(): pos > size()!");
        return BasicStringView(m_start + pos,
                               std::min(n, m_size - pos)).compare(str, size);
    }

    constexpr bool startsWith(CharT const * str, SizeType n) const noexcept
    { return (m_size >= n) && (compare(0u, n, str, n) == 0); }

    constexpr bool startsWith(BasicStringView v) const noexcept
    { return startsWith(v.m_start, v.m_size); }

    constexpr bool startsWith(CharT c) const noexcept
    { return (m_size > 0u) && (Traits::eq(front(), c)); }

    /// \todo Optimize not to iterate over str twice:
    constexpr bool startsWith(CharT const * str) const noexcept
    { return startsWith(str, Traits::length(str)); }

    constexpr bool endsWith(CharT const * str, SizeType n) const noexcept {
        return (m_size >= n) && (compare(m_size - n, n, str, n) == 0);
    }

    constexpr bool endsWith(BasicStringView v) const noexcept
    { return endsWith(v.m_start, v.m_size); }

    constexpr bool endsWith(CharT c) const noexcept
    { return (m_size > 0u) && (Traits::eq(back(), c)); }

    constexpr bool endsWith(CharT const * str) const noexcept
    { return endsWith(str, Traits::length(str)); }

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
    { return find(str, pos, Traits::length(str)); }

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
    { return rfind(str, pos, Traits::length(str)); }

    constexpr SizeType findFirstOf(CharT c, SizeType pos = 0u) const noexcept
    { return find(c, pos); }

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
    { return findFirstOf(str, pos, Traits::length(str)); }

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
    { return findFirstNotOf(str, pos, Traits::length(str)); }

    constexpr SizeType findLastOf(CharT c, SizeType pos = npos) const noexcept
    { return rfind(c, pos); }

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
    { return findLastOf(str, pos, Traits::length(str)); }

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
    { return findLastNotOf(str, pos, Traits::length(str)); }

    constexpr BasicStringView leftTrimmed(CharT c) const noexcept {
        for (SizeType i = 0u; i < m_size; ++i)
            if (!Traits::eq(m_start[i], c))
                return BasicStringView(m_start + i, m_size - i);
        return BasicStringView();
    }

    constexpr BasicStringView leftTrimmed(CharT const * cs, SizeType count)
            const noexcept
    {
        for (SizeType i = 0u; i < m_size; ++i)
            if (!Traits::find(cs, count, m_start[i]))
                return BasicStringView(m_start + i, m_size - i);
        return BasicStringView();
    }

    constexpr BasicStringView leftTrimmed(BasicStringView cs) const noexcept
    { return leftTrimmed(cs.m_start, cs.m_size); }

    constexpr BasicStringView leftTrimmed(CharT const * cs) const noexcept
    { return leftTrimmed(cs, Traits::length(cs)); }

    constexpr BasicStringView rightTrimmed(CharT c) const noexcept {
        for (SizeType i = m_size; i--;)
            if (!Traits::eq(m_start[i], c))
                return BasicStringView(m_start, i + 1u);
        return BasicStringView();
    }

    constexpr BasicStringView rightTrimmed(CharT const * cs, SizeType count)
            const noexcept
    {
        for (SizeType i = m_size; i--;)
            if (!Traits::find(cs, count, m_start[i]))
                return BasicStringView(m_start, i + 1u);
        return BasicStringView();
    }

    constexpr BasicStringView rightTrimmed(BasicStringView cs) const noexcept
    { return rightTrimmed(cs.m_start, cs.m_size); }

    constexpr BasicStringView rightTrimmed(CharT const * cs) const noexcept
    { return rightTrimmed(cs, Traits::length(cs)); }

    constexpr BasicStringView trimmed(CharT c) const noexcept {
        for (SizeType i = 0u; i < m_size; ++i) /* first inclusive */
            if (!Traits::eq(m_start[i], c))
                for (SizeType i2 = m_size - 1u;; --i2) /* last inclusive */
                    if (!Traits::eq(m_start[i2], c))
                        /* Return range [i, i2]: */
                        return BasicStringView(m_start + i, i2 + 1u - i);
        return BasicStringView();
    }

    constexpr BasicStringView trimmed(CharT const * cs, SizeType count)
            const noexcept
    {
        for (SizeType i = 0u; i < m_size; ++i) /* first inclusive */
            if (!Traits::find(cs, count, m_start[i]))
                for (SizeType i2 = m_size - 1u;; --i2) /* last inclusive */
                    if (!Traits::find(cs, count, m_start[i2]))
                        /* Return range [i, i2]: */
                        return BasicStringView(m_start + i, i2 + 1u - i);
        return BasicStringView();
    }

    constexpr BasicStringView trimmed(BasicStringView cs) const noexcept
    { return trimmed(cs.m_start, cs.m_size); }

    constexpr BasicStringView trimmed(CharT const * cs) const noexcept
    { return trimmed(cs, Traits::length(cs)); }

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
