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

#include "../src/StringView.h"

#include <algorithm>
#include <boost/functional/hash.hpp>
#include <iomanip>
#include <memory>
#include <sstream>
#include <type_traits>
#include <utility>
#include "../src/TestAssert.h"


using namespace sharemind::StringViewLiterals;

#define SA(...) static_assert(__VA_ARGS__, "")
#define SAS(...) SA(std::is_same<__VA_ARGS__>::value)
#define SASD(T,...) SAS(decltype(__VA_ARGS__), T)
#define D(...) std::declval<__VA_ARGS__>()

template <typename CharT>
void staticTests() {
    using SV = sharemind::BasicStringView<CharT>;
    #define SVR D(SV &)
    #define SVCR D(SV const &)

    SAS(typename SV::TraitsType, std::char_traits<CharT>);
    SAS(typename SV::ValueType, CharT);
    SAS(typename SV::Pointer, CharT *);
    SAS(typename SV::ConstPointer, CharT const *);
    SAS(typename SV::Reference, CharT &);
    SAS(typename SV::ConstReference, CharT const &);
    SAS(typename SV::Iterator, CharT const *);
    SAS(typename SV::ConstIterator, CharT const *);
    SAS(typename SV::ReverseIterator,
        std::reverse_iterator<typename SV::Iterator> );
    SAS(typename SV::ConstReverseIterator,
        std::reverse_iterator<typename SV::ConstIterator> );
    SAS(typename SV::SizeType, std::size_t);
    SAS(typename SV::DifferenceType, std::ptrdiff_t);
    SASD(typename SV::SizeType const, SV::npos);

    using ST = typename SV::SizeType;

    SA(SV::npos == ST(-1));

    SA(std::is_nothrow_default_constructible<SV>::value);
    SA(std::is_nothrow_copy_constructible<SV>::value);
    SA(std::is_nothrow_move_constructible<SV>::value);
    SA(std::is_nothrow_constructible<SV,
                                     std::basic_string<CharT> const &>::value);
    SA(std::is_nothrow_constructible<SV, CharT const *>::value);
    SA(std::is_nothrow_constructible<SV, CharT const *, ST>::value);
    SA(std::is_nothrow_copy_assignable<SV>::value);
    SA(std::is_nothrow_move_assignable<SV>::value);
    SA(std::is_trivially_destructible<SV>::value);

    #define SASD_NOEXCEPT(T,...) \
        SASD(T, __VA_ARGS__); \
        SA(noexcept(__VA_ARGS__))
    SASD_NOEXCEPT(typename SV::Iterator, SVR.begin());
    SASD_NOEXCEPT(typename SV::ConstIterator, SVCR.begin());
    SASD_NOEXCEPT(typename SV::ConstIterator, SVR.cbegin());
    SASD_NOEXCEPT(typename SV::ConstIterator, SVCR.cbegin());
    SASD_NOEXCEPT(typename SV::Iterator, SVR.end());
    SASD_NOEXCEPT(typename SV::ConstIterator, SVCR.end());
    SASD_NOEXCEPT(typename SV::ConstIterator, SVR.cend());
    SASD_NOEXCEPT(typename SV::ConstIterator, SVCR.cend());

    SASD_NOEXCEPT(typename SV::ReverseIterator, SVR.rbegin());
    SASD_NOEXCEPT(typename SV::ConstReverseIterator, SVCR.rbegin());
    SASD_NOEXCEPT(typename SV::ConstReverseIterator, SVR.crbegin());
    SASD_NOEXCEPT(typename SV::ConstReverseIterator, SVCR.crbegin());
    SASD_NOEXCEPT(typename SV::ReverseIterator, SVR.rend());
    SASD_NOEXCEPT(typename SV::ConstReverseIterator, SVCR.rend());
    SASD_NOEXCEPT(typename SV::ConstReverseIterator, SVR.crend());
    SASD_NOEXCEPT(typename SV::ConstReverseIterator, SVCR.crend());

    SASD_NOEXCEPT(ST, SVR.length());
    SASD_NOEXCEPT(ST, SVCR.length());
    SASD_NOEXCEPT(ST, SVR.size());
    SASD_NOEXCEPT(ST, SVCR.size());

    SA(std::is_convertible<decltype(SVR.empty()), bool>::value);
    SA(std::is_convertible<decltype(SVCR.empty()), bool>::value);

    SASD_NOEXCEPT(typename SV::ConstPointer, SVR.data());
    SASD_NOEXCEPT(typename SV::ConstPointer, SVCR.data());

    SASD_NOEXCEPT(typename SV::ConstReference, SVR.front());
    SASD_NOEXCEPT(typename SV::ConstReference, SVCR.front());
    SASD_NOEXCEPT(typename SV::ConstReference, SVR.back());
    SASD_NOEXCEPT(typename SV::ConstReference, SVCR.back());

    SASD_NOEXCEPT(typename SV::ConstReference, SVR[D(ST)]);
    SASD_NOEXCEPT(typename SV::ConstReference, SVCR[D(ST)]);

    SASD(typename SV::ConstReference, SVR.at(D(ST)));
    SASD(typename SV::ConstReference, SVCR.at(D(ST)));

    SASD_NOEXCEPT(void, SVR.removePrefix(D(ST)));
    SASD_NOEXCEPT(void, SVR.removeSuffix(D(ST)));
    SASD_NOEXCEPT(void, SVR.swap(SVR));

    SASD(SV, SVR.substr(D(ST)));
    SASD(SV, SVCR.substr(D(ST)));
    SASD(SV, SVR.substr(D(ST), D(ST)));
    SASD(SV, SVCR.substr(D(ST), D(ST)));

    SASD(SV, SVR.left(D(ST)));
    SASD(SV, SVCR.left(D(ST)));

    SASD(SV, SVR.right(D(ST)));
    SASD(SV, SVCR.right(D(ST)));

    SASD(SV, SVR.leftClipped(D(ST)));
    SASD(SV, SVCR.leftClipped(D(ST)));

    SASD(SV, SVR.rightClipped(D(ST)));
    SASD(SV, SVCR.rightClipped(D(ST)));

    SASD(SV, SVR.clipped(D(ST), D(ST)));
    SASD(SV, SVCR.clipped(D(ST), D(ST)));

    #define SA_SIGNED(...) SA(std::is_signed<__VA_ARGS__>::value)
    #define SA_SIGNED_NOEXCEPT(...) SA_SIGNED(decltype(__VA_ARGS__)); \
                                    SA(noexcept(__VA_ARGS__))
    SA_SIGNED_NOEXCEPT(SVR.compare(SVR));
    SA_SIGNED_NOEXCEPT(SVR.compare(SVCR));
    SA_SIGNED_NOEXCEPT(SVCR.compare(SVR));
    SA_SIGNED_NOEXCEPT(SVCR.compare(SVCR));
    SA_SIGNED_NOEXCEPT(SVR.compare(D(ST), D(ST), SVR));
    SA_SIGNED_NOEXCEPT(SVR.compare(D(ST), D(ST), SVCR));
    SA_SIGNED_NOEXCEPT(SVCR.compare(D(ST), D(ST), SVR));
    SA_SIGNED_NOEXCEPT(SVCR.compare(D(ST), D(ST), SVCR));
    SA_SIGNED_NOEXCEPT(SVR.compare(D(ST), D(ST), SVR, D(ST), D(ST)));
    SA_SIGNED_NOEXCEPT(SVR.compare(D(ST), D(ST), SVCR, D(ST), D(ST)));
    SA_SIGNED_NOEXCEPT(SVCR.compare(D(ST), D(ST), SVR, D(ST), D(ST)));
    SA_SIGNED_NOEXCEPT(SVCR.compare(D(ST), D(ST), SVCR, D(ST), D(ST)));
    SA_SIGNED_NOEXCEPT(SVR.compare(D(CharT const *)));
    SA_SIGNED_NOEXCEPT(SVCR.compare(D(CharT const *)));
    SA_SIGNED_NOEXCEPT(SVR.compare(D(ST), D(ST), D(CharT const *)));
    SA_SIGNED_NOEXCEPT(SVCR.compare(D(ST), D(ST), D(CharT const *)));
    SA_SIGNED_NOEXCEPT(SVR.compare(D(ST), D(ST), D(CharT const *), D(ST)));
    SA_SIGNED_NOEXCEPT(SVCR.compare(D(ST), D(ST), D(CharT const *), D(ST)));
    #undef SA_SIGNED_NOEXCEPT
    #undef SA_SIGNED

    SASD_NOEXCEPT(bool, SVR.startsWith(SVR));
    SASD_NOEXCEPT(bool, SVR.startsWith(SVCR));
    SASD_NOEXCEPT(bool, SVCR.startsWith(SVR));
    SASD_NOEXCEPT(bool, SVCR.startsWith(SVCR));
    SASD_NOEXCEPT(bool, SVR.startsWith(D(CharT)));
    SASD_NOEXCEPT(bool, SVCR.startsWith(D(CharT)));
    SASD_NOEXCEPT(bool, SVR.startsWith(D(CharT const *)));
    SASD_NOEXCEPT(bool, SVCR.startsWith(D(CharT const *)));

    SASD_NOEXCEPT(bool, SVR.endsWith(SVR));
    SASD_NOEXCEPT(bool, SVR.endsWith(SVCR));
    SASD_NOEXCEPT(bool, SVCR.endsWith(SVR));
    SASD_NOEXCEPT(bool, SVCR.endsWith(SVCR));
    SASD_NOEXCEPT(bool, SVR.endsWith(D(CharT)));
    SASD_NOEXCEPT(bool, SVCR.endsWith(D(CharT)));
    SASD_NOEXCEPT(bool, SVR.endsWith(D(CharT const *)));
    SASD_NOEXCEPT(bool, SVCR.endsWith(D(CharT const *)));

    SASD_NOEXCEPT(ST, SVR.find(D(CharT)));
    SASD_NOEXCEPT(ST, SVCR.find(D(CharT)));
    SASD_NOEXCEPT(ST, SVR.find(D(CharT), D(ST)));
    SASD_NOEXCEPT(ST, SVCR.find(D(CharT), D(ST)));

    SASD_NOEXCEPT(ST, SVR.rfind(D(CharT)));
    SASD_NOEXCEPT(ST, SVCR.rfind(D(CharT)));
    SASD_NOEXCEPT(ST, SVR.rfind(D(CharT), D(ST)));
    SASD_NOEXCEPT(ST, SVCR.rfind(D(CharT), D(ST)));

    SASD(ST, SVR.copy(D(CharT *), D(ST)));
    SASD(ST, SVCR.copy(D(CharT *), D(ST)));
    SASD(ST, SVR.copy(D(CharT *), D(ST), D(ST)));
    SASD(ST, SVCR.copy(D(CharT *), D(ST), D(ST)));

    #define TEST_OP(op) \
        SASD_NOEXCEPT(bool, SVR op SVR); \
        SASD_NOEXCEPT(bool, SVR op SVCR); \
        SASD_NOEXCEPT(bool, SVCR op SVR); \
        SASD_NOEXCEPT(bool, SVCR op SVCR); \
        SASD_NOEXCEPT(bool, SVR op D(CharT const *)); \
        SASD_NOEXCEPT(bool, SVCR op D(CharT const *)); \
        SASD_NOEXCEPT(bool, D(CharT const *) op SVR); \
        SASD_NOEXCEPT(bool, D(CharT const *) op SVCR); \
        SASD_NOEXCEPT(bool, SVR op D(std::basic_string<CharT> const &)); \
        SASD_NOEXCEPT(bool, SVCR op D(std::basic_string<CharT> const &)); \
        SASD_NOEXCEPT(bool, D(std::basic_string<CharT> const &) op SVR); \
        SASD_NOEXCEPT(bool, D(std::basic_string<CharT> const &) op SVCR);
    TEST_OP(==) TEST_OP(!=) TEST_OP(<) TEST_OP(<=) TEST_OP(>=) TEST_OP(>)
    #undef TEST_OP

    SASD(std::size_t, boost::hash<SV>()(SVR));
    SASD(std::size_t, boost::hash<SV>()(SVCR));

    #undef SASD_NOEXCEPT
    #undef SVR
    #undef SVCR
}

SAS(sharemind::BasicStringView<char>, sharemind::StringView);
SAS(sharemind::BasicStringView<wchar_t>, sharemind::WideStringView);
SAS(sharemind::BasicStringView<char16_t>, sharemind::U16StringView);
SAS(sharemind::BasicStringView<char32_t>, sharemind::U32StringView);

SASD(sharemind::StringView, "asdf"_sv);
SASD(sharemind::WideStringView, L"asdf"_sv);
SASD(sharemind::U16StringView, u"asdf"_sv);
SASD(sharemind::U32StringView, U"asdf"_sv);

int main() {
    staticTests<char>();
    staticTests<wchar_t>();
    staticTests<char16_t>();
    staticTests<char32_t>();

    using SV = sharemind::StringView;
    using namespace sharemind::StringViewLiterals;

    static auto const newConstSv =
            [](char const * str) {
                struct Bind {
                    Bind(char const * str) : s(str) {}
                    std::string s;
                    SV sv{s};
                };
                auto b(std::make_shared<Bind>(str));
                return std::shared_ptr<SV const>(b, &b->sv);
            };

    auto const hv("Hello, World!"_sv);
    auto const hv2(newConstSv("Hello, World!"));
    {
        SV const tmp("Hello, World!", 5u);
        SHAREMIND_TESTASSERT(tmp == "Hello");
    }
    {
        std::string const hello("Hello");
        SV const tmp(hello);
        SHAREMIND_TESTASSERT(tmp == "Hello");
    }
    SHAREMIND_TESTASSERT(!hv.empty());
    SHAREMIND_TESTASSERT(""_sv.empty());
    SHAREMIND_TESTASSERT(hv.size() == sizeof("Hello, World!") - 1u);
    SHAREMIND_TESTASSERT(""_sv.size() == 0u);
    SHAREMIND_TESTASSERT(hv.length() == sizeof("Hello, World!") - 1u);
    SHAREMIND_TESTASSERT(""_sv.length() == 0u);

    SHAREMIND_TESTASSERT(hv.front() == 'H');
    SHAREMIND_TESTASSERT(hv[0u] == 'H');
    SHAREMIND_TESTASSERT(hv[1u] == 'e');
    SHAREMIND_TESTASSERT(hv[2u] == 'l');
    SHAREMIND_TESTASSERT(hv[3u] == 'l');
    SHAREMIND_TESTASSERT(hv[4u] == 'o');
    SHAREMIND_TESTASSERT(hv[5u] == ',');
    SHAREMIND_TESTASSERT(hv[6u] == ' ');
    SHAREMIND_TESTASSERT(hv[7u] == 'W');
    SHAREMIND_TESTASSERT(hv[8u] == 'o');
    SHAREMIND_TESTASSERT(hv[9u] == 'r');
    SHAREMIND_TESTASSERT(hv[10u] == 'l');
    SHAREMIND_TESTASSERT(hv[11u] == 'd');
    SHAREMIND_TESTASSERT(hv[12u] == '!');
    SHAREMIND_TESTASSERT(hv.back() == '!');
    for (SV::SizeType i = 0u; i < hv.size(); ++i)
        SHAREMIND_TESTASSERT(hv[i] == hv.at(i));
    SHAREMIND_TEST_THROWS(hv.at(hv.size()));

    #define REMOVED(what,i) \
        []() noexcept { \
            auto v("0123"_sv); \
            v.remove ## what(i); \
            return v; \
        }()
    SHAREMIND_TESTASSERT(REMOVED(Prefix, 0u) == "0123");
    SHAREMIND_TESTASSERT(REMOVED(Prefix, 1u) == "123");
    SHAREMIND_TESTASSERT(REMOVED(Prefix, 2u) == "23");
    SHAREMIND_TESTASSERT(REMOVED(Prefix, 3u) == "3");
    SHAREMIND_TESTASSERT(REMOVED(Prefix, 4u).empty());
    SHAREMIND_TESTASSERT(REMOVED(Suffix, 0u) == "0123");
    SHAREMIND_TESTASSERT(REMOVED(Suffix, 1u) == "012");
    SHAREMIND_TESTASSERT(REMOVED(Suffix, 2u) == "01");
    SHAREMIND_TESTASSERT(REMOVED(Suffix, 3u) == "0");
    SHAREMIND_TESTASSERT(REMOVED(Suffix, 4u).empty());
    #undef REMOVED

    {
        auto a("A"_sv);
        auto b("B"_sv);
        a.swap(b);
        SHAREMIND_TESTASSERT(a == "B");
        SHAREMIND_TESTASSERT(b == "A");
        std::swap(a, b);
        SHAREMIND_TESTASSERT(a == "A");
        SHAREMIND_TESTASSERT(b == "B");
    }

    SHAREMIND_TESTASSERT(hv.substr(0u, 1u) == "H");
    SHAREMIND_TESTASSERT(hv.substr(0u, 2u) == "He");
    SHAREMIND_TESTASSERT(hv.substr(0u, 3u) == "Hel");
    SHAREMIND_TESTASSERT(hv.substr(1u, 3u) == "ell");
    SHAREMIND_TESTASSERT(hv.substr(2u, 3u) == "llo");
    SHAREMIND_TESTASSERT(hv.substr(10u, 1u) == "l");
    SHAREMIND_TESTASSERT(hv.substr(10u, 2u) == "ld");
    SHAREMIND_TESTASSERT(hv.substr(10u, 3u) == "ld!");
    SHAREMIND_TESTASSERT(hv.substr(10u, 4u) == "ld!");
    SHAREMIND_TESTASSERT(hv.substr(10u, 5u) == "ld!");
    SHAREMIND_TESTASSERT(hv.substr(10u) == "ld!");
    SHAREMIND_TESTASSERT(hv.substr(hv.size()).empty());
    SHAREMIND_TEST_THROWS(hv.substr(hv.size() + 1u));
    SHAREMIND_TEST_THROWS(hv.substr(hv.size() + 2u));

    for (SV::SizeType i = 0u; i <= hv.size(); ++i)
        SHAREMIND_TESTASSERT(hv.left(i) == hv.substr(0u, i));
    SHAREMIND_TEST_THROWS(hv.left(hv.size() + 1u));
    SHAREMIND_TEST_THROWS(hv.left(hv.size() + 2u));

    for (SV::SizeType i = 0u; i <= hv.size(); ++i)
        SHAREMIND_TESTASSERT(hv.right(i) == hv.substr(hv.size() - i, i));
    SHAREMIND_TEST_THROWS(hv.right(hv.size() + 1u));
    SHAREMIND_TEST_THROWS(hv.right(hv.size() + 2u));

    for (SV::SizeType i = 0u; i <= hv.size(); ++i)
        SHAREMIND_TESTASSERT(hv.leftClipped(i) == hv.substr(i));
    SHAREMIND_TEST_THROWS(hv.leftClipped(hv.size() + 1u));
    SHAREMIND_TEST_THROWS(hv.leftClipped(hv.size() + 2u));

    for (SV::SizeType i = 0u; i <= hv.size(); ++i)
        SHAREMIND_TESTASSERT(hv.rightClipped(i) == hv.substr(0u, hv.size() - i));
    SHAREMIND_TEST_THROWS(hv.rightClipped(hv.size() + 1u));
    SHAREMIND_TEST_THROWS(hv.rightClipped(hv.size() + 2u));

    for (SV::SizeType l = 0u; l <= hv.size() + 2u; ++l) {
        for (SV::SizeType r = 0u; r <= hv.size() + 2u; ++r) {
            if ((l > hv.size()) || (r > hv.size() - l)) {
                SHAREMIND_TEST_THROWS(hv.clipped(l, r));
            } else {
                SHAREMIND_TESTASSERT(hv.clipped(l, r)
                                     == hv.leftClipped(l).rightClipped(r));
            }
        }
    }

    #define TEST_COMPARE(a,op,...) \
        SHAREMIND_TESTASSERT((a).compare(__VA_ARGS__) op 0); \
        SHAREMIND_TESTASSERT(((a) op __VA_ARGS__) \
                             == ((a).compare(__VA_ARGS__) op 0))
    TEST_COMPARE(""_sv,==,""_sv);
    TEST_COMPARE(""_sv,==,*newConstSv(""));
    TEST_COMPARE(hv,==,hv);
    TEST_COMPARE(hv,==,*hv2);
    TEST_COMPARE(""_sv,<,hv);
    TEST_COMPARE(*newConstSv(""),<,hv);
    TEST_COMPARE(hv,>,*newConstSv(""));
    TEST_COMPARE(hv,>,SV(hv).rightClipped(1u));
    TEST_COMPARE(hv,>,SV(*hv2).rightClipped(1u));
    TEST_COMPARE(SV(hv).rightClipped(1u),<,hv);
    TEST_COMPARE(SV(hv).rightClipped(1u),<,*hv2);
    TEST_COMPARE("aaa"_sv,<,"aab"_sv);
    TEST_COMPARE("aaa"_sv,<,"aba"_sv);
    TEST_COMPARE("aaa"_sv,<,"baa"_sv);
    TEST_COMPARE("aab"_sv,>,"aaa"_sv);
    TEST_COMPARE("aba"_sv,>,"aaa"_sv);
    TEST_COMPARE("baa"_sv,>,"aaa"_sv);

    TEST_COMPARE(""_sv,<=,""_sv);
    TEST_COMPARE(""_sv,<=,*newConstSv(""));
    TEST_COMPARE(hv,<=,hv);
    TEST_COMPARE(hv,<=,*hv2);
    TEST_COMPARE(""_sv,<=,hv);
    TEST_COMPARE(*newConstSv(""),<=,hv);
    TEST_COMPARE(SV(hv).rightClipped(1u),<=,hv);
    TEST_COMPARE(SV(hv).rightClipped(1u),<=,*hv2);
    TEST_COMPARE("aaa"_sv,<=,"aab"_sv);
    TEST_COMPARE("aaa"_sv,<=,"aba"_sv);
    TEST_COMPARE("aaa"_sv,<=,"baa"_sv);

    TEST_COMPARE(""_sv,>=,""_sv);
    TEST_COMPARE(""_sv,>=,*newConstSv(""));
    TEST_COMPARE(hv,>=,hv);
    TEST_COMPARE(hv,>=,*hv2);
    TEST_COMPARE(hv,>=,*newConstSv(""));
    TEST_COMPARE(hv,>=,SV(hv).rightClipped(1u));
    TEST_COMPARE(hv,>=,SV(*hv2).rightClipped(1u));
    TEST_COMPARE("aab"_sv,>=,"aaa"_sv);
    TEST_COMPARE("aba"_sv,>=,"aaa"_sv);
    TEST_COMPARE("baa"_sv,>=,"aaa"_sv);

    TEST_COMPARE(""_sv,!=,hv);
    TEST_COMPARE(*newConstSv(""),!=,hv);
    TEST_COMPARE(hv,!=,*newConstSv(""));
    TEST_COMPARE(hv,!=,SV(hv).rightClipped(1u));
    TEST_COMPARE(hv,!=,SV(*hv2).rightClipped(1u));
    TEST_COMPARE(SV(hv).rightClipped(1u),!=,hv);
    TEST_COMPARE(SV(hv).rightClipped(1u),!=,*hv2);
    TEST_COMPARE("aaa"_sv,!=,"aab"_sv);
    TEST_COMPARE("aaa"_sv,!=,"aba"_sv);
    TEST_COMPARE("aaa"_sv,!=,"baa"_sv);
    TEST_COMPARE("aab"_sv,!=,"aaa"_sv);
    TEST_COMPARE("aba"_sv,!=,"aaa"_sv);
    TEST_COMPARE("baa"_sv,!=,"aaa"_sv);
    #undef TEST_COMPARE

    SHAREMIND_TESTASSERT(hv.startsWith('H'));
    SHAREMIND_TESTASSERT(!hv.startsWith('e'));
    SHAREMIND_TESTASSERT(!hv.startsWith('!'));

    SHAREMIND_TESTASSERT(hv.startsWith("Hello"));
    SHAREMIND_TESTASSERT(!hv.startsWith("ello"));
    SHAREMIND_TESTASSERT(!hv.startsWith("World!"));

    SHAREMIND_TESTASSERT(hv.startsWith("Hello"_sv));
    SHAREMIND_TESTASSERT(!hv.startsWith("ello"_sv));
    SHAREMIND_TESTASSERT(!hv.startsWith("World!"_sv));

    SHAREMIND_TESTASSERT(hv.endsWith('!'));
    SHAREMIND_TESTASSERT(!hv.endsWith('d'));
    SHAREMIND_TESTASSERT(!hv.endsWith('H'));

    SHAREMIND_TESTASSERT(hv.endsWith("World!"));
    SHAREMIND_TESTASSERT(!hv.endsWith("World"));
    SHAREMIND_TESTASSERT(!hv.endsWith("Hello"));

    SHAREMIND_TESTASSERT(hv.endsWith("World!"_sv));
    SHAREMIND_TESTASSERT(!hv.endsWith("World"_sv));
    SHAREMIND_TESTASSERT(!hv.endsWith("Hello"_sv));

    {
        auto ev(""_sv);
        auto const cev(ev);
        SHAREMIND_TESTASSERT(ev.find('x') == SV::npos);
        SHAREMIND_TESTASSERT(cev.find('x') == SV::npos);
        SHAREMIND_TESTASSERT(ev.rfind('x') == SV::npos);
        SHAREMIND_TESTASSERT(cev.rfind('x') == SV::npos);
        SHAREMIND_TESTASSERT(ev.findFirstOf('x') == SV::npos);
        SHAREMIND_TESTASSERT(cev.findFirstOf('x') == SV::npos);
        SHAREMIND_TESTASSERT(ev.findLastOf('x') == SV::npos);
        SHAREMIND_TESTASSERT(cev.findLastOf('x') == SV::npos);
        SHAREMIND_TESTASSERT(ev.findFirstNotOf('x') == SV::npos);
        SHAREMIND_TESTASSERT(cev.findFirstNotOf('x') == SV::npos);
        SHAREMIND_TESTASSERT(ev.findLastNotOf('x') == SV::npos);
        SHAREMIND_TESTASSERT(cev.findLastNotOf('x') == SV::npos);
    }
    {
        static_assert(std::string::npos == SV::npos, "");
        std::string const snums("01234m01234");
        SV nums(snums);
        auto const cnums(nums);
        #define TESTS \
            TEST(find,find); \
            TEST(rfind,rfind); \
            TEST(findFirstOf,find_first_of); \
            TEST(findLastOf,find_last_of); \
            TEST(findFirstNotOf,find_first_not_of); \
            TEST(findLastNotOf,find_last_not_of)
        for (auto c : "x01234m"_sv) {
            #define TEST(m,sm) SHAREMIND_TESTASSERT(nums.m(c) == snums.sm(c)); \
                               SHAREMIND_TESTASSERT(cnums.m(c) == snums.sm(c))
            TESTS;
            #undef TEST
            for (SV::SizeType i = 0u; i < nums.size() + 3u; ++i) {
                #define TEST(m,sm) \
                    SHAREMIND_TESTASSERT(nums.m(c, i) == snums.sm(c, i)); \
                    SHAREMIND_TESTASSERT(cnums.m(c, i) == snums.sm(c, i))
                TESTS;
                #undef TEST
            }
        }
        #undef TESTS
    }
    {
        using S = SV::SizeType;
        std::string const snums("012345");
        SV nums(snums);
        auto const cnums(nums);
        #define SUB nums.substr(subPos, subSize)
        #define TEST_(m,sm,...) \
            do { \
                auto const expected(snums.sm(__VA_ARGS__)); \
                SHAREMIND_TESTASSERT(nums.m(__VA_ARGS__) == expected); \
                SHAREMIND_TESTASSERT(cnums.m(__VA_ARGS__) == expected); \
            } while(false)
        #define TEST(...) \
            TEST_(find, find, __VA_ARGS__); \
            TEST_(rfind, rfind, __VA_ARGS__); \
            TEST_(findFirstOf, find_first_of, __VA_ARGS__); \
            TEST_(findLastOf, find_last_of, __VA_ARGS__); \
            TEST_(findFirstNotOf, find_first_not_of, __VA_ARGS__); \
            TEST_(findLastNotOf, find_last_not_of, __VA_ARGS__);
        for (S subPos = 0u; subPos <= snums.size(); ++subPos) {
            for (S count = 0u; count < snums.size() - subPos; ++count) {
                for (S pos = 0u; pos < snums.size() + 3u; ++pos) {
                    auto const subStr(snums.substr(subPos, count));
                    auto const subStr2(subStr + "x");
                    auto const subStr3("x" + subStr);

                    TEST(subStr.c_str(),  pos, subStr.size());
                    TEST(subStr2.c_str(), pos, subStr2.size());
                    TEST(subStr2.c_str(), pos, count);
                    TEST(subStr2.c_str() + 1u, pos, count);
                    TEST(subStr3.c_str(), pos, subStr3.size());
                    TEST(subStr3.c_str(), pos, count);
                    TEST(subStr3.c_str() + 1u, pos, count);

                    TEST(subStr.c_str(),  pos);
                    TEST(subStr2.c_str(), pos);
                    TEST(subStr2.c_str() + 1u, pos);
                    TEST(subStr3.c_str(), pos);
                    TEST(subStr3.c_str() + 1u, pos);

                    #undef TEST_
                    #define TEST_(m,sm,...) \
                        do { \
                            SV const sv(__VA_ARGS__); \
                            auto const expected(snums.sm(sv.data(), pos, sv.size())); \
                            SHAREMIND_TESTASSERT(nums.m(sv, pos) == expected); \
                            SHAREMIND_TESTASSERT(cnums.m(sv, pos) == expected); \
                        } while(false)
                    TEST(subStr);
                    TEST(subStr2);
                    TEST(subStr2.c_str(), count);
                    TEST(subStr2.c_str() + 1u, count);
                    TEST(subStr3);
                    TEST(subStr3.c_str(), count);
                    TEST(subStr3.c_str() + 1u, count);
                }
            }
        }
        #undef TEST
        #undef TEST_
        #undef SUB
    }
    {
        std::string const snums("01234");
        auto const s = snums.size();
        SV const nums(snums);
        auto r1(std::make_unique<SV::ValueType[]>(s + 1u));
        auto r2(std::make_unique<SV::ValueType[]>(s + 1u));
        for (SV::SizeType subPos = 0u; subPos <= s; ++subPos) {
            for (SV::SizeType count = 0u; count <= s; ++count) {
                std::fill(r1.get(), r1.get() + s + 1u, 'x');
                std::fill(r2.get(), r2.get() + s + 1u, 'x');

                SHAREMIND_TESTASSERT(snums.copy(r1.get(), count, subPos)
                                     == nums.copy(r2.get(), count, subPos));
                SHAREMIND_TESTASSERT(
                            std::equal(r1.get(), r1.get() + s + 1u, r2.get()));
                SHAREMIND_TESTASSERT(r1[s] == 'x');
            }
        }
    }

    {
        std::ostringstream oss;
        oss << hv;
        SHAREMIND_TESTASSERT(oss.str() == hv);
        SHAREMIND_TESTASSERT(hv == oss.str());
    }{
        std::ostringstream oss;
        oss << std::setfill('.')
            << std::right << std::setw(10) << SV("01234") << '-'
            << std::left  << std::setw(10) << SV("01234");
        SHAREMIND_TESTASSERT(oss.str() == ".....01234-01234.....");
    }

    boost::hash<SV> hasher;
    SHAREMIND_TESTASSERT(hasher(hv) == hasher("Hello, World!"));
    SHAREMIND_TESTASSERT(hasher(hv) != hasher("Hello, James!"));
    SHAREMIND_TESTASSERT(hasher(hv) == hasher(*hv2));
}
