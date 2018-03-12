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

#include "../src/UnorderedMap.h"

#include <cstddef>
#include <cstring>
#include <iterator>
#include <random>
#include <unordered_set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "../src/TestAssert.h"
#include "../src/StringHasher.h"
#include "../src/StringHashTablePredicate.h"


namespace {

struct Rand {
    std::random_device rd;
    std::mt19937 rng{rd()};
    std::uniform_int_distribution<unsigned> dist;

    unsigned get() noexcept { return dist(rng); }
} * myRand;

constexpr std::size_t const ITERATIONS = 100u;
static_assert(ITERATIONS > 0u, "");
static_assert(ITERATIONS % 4u == 0u, "");

struct X1 {}; struct X2 {}; struct X3{};

#define SA(...) static_assert(__VA_ARGS__, "")

#define TEST_TRAIT(trait, ...) SA(std::trait<__VA_ARGS__>::value)
#define TEST_IS_SAME(a,b) TEST_TRAIT(is_same, a, b)
#define TEST_SAME_FIELD(fn) TEST_IS_SAME(typename UM::fn, typename RUM::fn)
#define TEST_IT_TRAIT_EQ_(it, fn) \
    TEST_IS_SAME(typename std::iterator_traits<typename UM::it>::fn, \
                 typename std::iterator_traits<typename RUM::it>::fn)
#define TEST_IT_TRAIT_EQ(it) \
    TEST_IT_TRAIT_EQ_(it, difference_type); \
    TEST_IT_TRAIT_EQ_(it, value_type); \
    TEST_IT_TRAIT_EQ_(it, iterator_category); \
    TEST_IT_TRAIT_EQ_(it, reference); \
    TEST_IT_TRAIT_EQ_(it, pointer)

#define TEST_IS_CONSTRUCTIBLE(...) SA(std::is_constructible<__VA_ARGS__>::value)

#define TEST_IS_ASSIGNABLE(...) SA(std::is_assignable<__VA_ARGS__>::value)

#define D(...) std::declval<__VA_ARGS__>()
#define TEST_MEMBER_RETURN(T, r, ...) \
    TEST_TRAIT(is_same, r, decltype(D(T).__VA_ARGS__))

template <typename UM>
struct Type {
    using type = UM;
    using K = typename UM::key_type;
    using V = typename UM::value_type;
    using M = typename UM::mapped_type;
    using S = typename UM::size_type;
    using H = typename UM::hasher;
    using P = typename UM::key_equal;
    using A = typename UM::allocator_type;
    using IT = typename UM::iterator;
    using CIT = typename UM::const_iterator;
    using LIT = typename UM::local_iterator;
    using CLIT = typename UM::const_local_iterator;

    using I = V const *;
    using IL = std::initializer_list<V>;

    static_assert(std::is_same<std::pair<K const, M>, V>::value, "");


    // Construct/destroy/copy:

    TEST_IS_CONSTRUCTIBLE(UM);
    TEST_IS_CONSTRUCTIBLE(UM, S);
    TEST_IS_CONSTRUCTIBLE(UM, S, H const &);
    TEST_IS_CONSTRUCTIBLE(UM, S, H const &, P const &);
    TEST_IS_CONSTRUCTIBLE(UM, S, H const &, P const &, A const &);

    TEST_IS_CONSTRUCTIBLE(UM, I, I);
    TEST_IS_CONSTRUCTIBLE(UM, I, I, S);
    TEST_IS_CONSTRUCTIBLE(UM, I, I, S, H const &);
    TEST_IS_CONSTRUCTIBLE(UM, I, I, S, H const &, P const &);
    TEST_IS_CONSTRUCTIBLE(UM, I, I, S, H const &, P const &, A const &);
    TEST_IS_CONSTRUCTIBLE(UM, IT, IT);
    TEST_IS_CONSTRUCTIBLE(UM, IT, IT, S);
    TEST_IS_CONSTRUCTIBLE(UM, IT, IT, S, H const &);
    TEST_IS_CONSTRUCTIBLE(UM, IT, IT, S, H const &, P const &);
    TEST_IS_CONSTRUCTIBLE(UM, IT, IT, S, H const &, P const &, A const &);
    TEST_IS_CONSTRUCTIBLE(UM, CIT, CIT);
    TEST_IS_CONSTRUCTIBLE(UM, CIT, CIT, S);
    TEST_IS_CONSTRUCTIBLE(UM, CIT, CIT, S, H const &);
    TEST_IS_CONSTRUCTIBLE(UM, CIT, CIT, S, H const &, P const &);
    TEST_IS_CONSTRUCTIBLE(UM, CIT, CIT, S, H const &, P const &, A const &);
    TEST_IS_CONSTRUCTIBLE(UM, LIT, LIT);
    TEST_IS_CONSTRUCTIBLE(UM, LIT, LIT, S);
    TEST_IS_CONSTRUCTIBLE(UM, LIT, LIT, S, H const &);
    TEST_IS_CONSTRUCTIBLE(UM, LIT, LIT, S, H const &, P const &);
    TEST_IS_CONSTRUCTIBLE(UM, LIT, LIT, S, H const &, P const &, A const &);
    TEST_IS_CONSTRUCTIBLE(UM, CLIT, CLIT);
    TEST_IS_CONSTRUCTIBLE(UM, CLIT, CLIT, S);
    TEST_IS_CONSTRUCTIBLE(UM, CLIT, CLIT, S, H const &);
    TEST_IS_CONSTRUCTIBLE(UM, CLIT, CLIT, S, H const &, P const &);
    TEST_IS_CONSTRUCTIBLE(UM, CLIT, CLIT, S, H const &, P const &, A const &);

    TEST_IS_CONSTRUCTIBLE(UM, UM const &);
    TEST_IS_CONSTRUCTIBLE(UM, UM &&);

    TEST_IS_CONSTRUCTIBLE(UM, A const &);
    TEST_IS_CONSTRUCTIBLE(UM, UM const &, A const &);
    TEST_IS_CONSTRUCTIBLE(UM, UM &&, A const &);

    TEST_IS_CONSTRUCTIBLE(UM, IL);
    TEST_IS_CONSTRUCTIBLE(UM, IL, S);
    TEST_IS_CONSTRUCTIBLE(UM, IL, S, H const &);
    TEST_IS_CONSTRUCTIBLE(UM, IL, S, H const &, P const &);
    TEST_IS_CONSTRUCTIBLE(UM, IL, S, H const &, P const &, A const &);

    SA(std::is_nothrow_destructible<UM>::value);

    TEST_IS_ASSIGNABLE(UM, UM const &);
    TEST_IS_ASSIGNABLE(UM, UM &&);
    TEST_IS_ASSIGNABLE(UM, IL);

    TEST_MEMBER_RETURN(UM &, typename UM::allocator_type, get_allocator());
    TEST_MEMBER_RETURN(UM const &, typename UM::allocator_type, get_allocator());


    // Size and capacity:

    TEST_MEMBER_RETURN(UM &, bool, empty());
    TEST_MEMBER_RETURN(UM const &, bool, empty());
    TEST_MEMBER_RETURN(UM &, typename UM::size_type, size());
    TEST_MEMBER_RETURN(UM const &, typename UM::size_type, size());
    TEST_MEMBER_RETURN(UM &, typename UM::size_type, max_size());
    TEST_MEMBER_RETURN(UM const &, typename UM::size_type, max_size());


    // Iterators:

    TEST_MEMBER_RETURN(UM &, typename UM::iterator, begin());
    TEST_MEMBER_RETURN(UM const &, typename UM::const_iterator, begin());
    TEST_MEMBER_RETURN(UM &, typename UM::const_iterator, cbegin());
    TEST_MEMBER_RETURN(UM const &, typename UM::const_iterator, cbegin());

    TEST_MEMBER_RETURN(UM &, typename UM::iterator, end());
    TEST_MEMBER_RETURN(UM const &, typename UM::const_iterator, end());
    TEST_MEMBER_RETURN(UM &, typename UM::const_iterator, cend());
    TEST_MEMBER_RETURN(UM const &, typename UM::const_iterator, cend());


    // Modifiers:

    using ITBP = std::pair<typename UM::iterator, bool>;
    TEST_MEMBER_RETURN(UM &, ITBP, emplace());
    TEST_MEMBER_RETURN(UM &, ITBP, emplace(D(X1)));
    TEST_MEMBER_RETURN(UM &, ITBP, emplace(D(X1), D(X2)));
    TEST_MEMBER_RETURN(UM &, ITBP, emplace(D(X1), D(X2), D(X3)));
    TEST_MEMBER_RETURN(UM &, IT, emplace_hint(D(CIT)));
    TEST_MEMBER_RETURN(UM &, IT, emplace_hint(D(CIT), D(X1)));
    TEST_MEMBER_RETURN(UM &, IT, emplace_hint(D(CIT), D(X1), D(X2)));
    TEST_MEMBER_RETURN(UM &, IT, emplace_hint(D(CIT), D(X1), D(X2), D(X3)));
    TEST_MEMBER_RETURN(UM &, ITBP, insert(D(V const &)));
    TEST_MEMBER_RETURN(UM &, ITBP, insert(D(V &&)));
    TEST_MEMBER_RETURN(UM &, IT, insert(D(CIT), D(V const &)));
    TEST_MEMBER_RETURN(UM &, IT, insert(D(CIT), D(V &&)));
    TEST_MEMBER_RETURN(UM &, void, insert(D(I), D(I)));
    TEST_MEMBER_RETURN(UM &, void, insert(D(IT), D(IT)));
    TEST_MEMBER_RETURN(UM &, void, insert(D(CIT), D(CIT)));
    TEST_MEMBER_RETURN(UM &, void, insert(D(IL)));

    TEST_MEMBER_RETURN(UM &, IT, erase(D(CIT)));
    TEST_MEMBER_RETURN(UM &, S, erase(D(K const &)));
    TEST_MEMBER_RETURN(UM &, IT, erase(D(CIT), D(CIT)));
    TEST_MEMBER_RETURN(UM &, void, clear());

    TEST_MEMBER_RETURN(UM &, void, swap(D(UM &)));


    // Observers:

    TEST_MEMBER_RETURN(UM const &, H, hash_function());
    TEST_MEMBER_RETURN(UM const &, P, key_eq());


    // Lookup:

    using ITP = std::pair<IT, IT>;
    using CITP = std::pair<CIT, CIT>;
    TEST_MEMBER_RETURN(UM &, IT, find(D(K const &)));
    TEST_MEMBER_RETURN(UM const &, CIT, find(D(K const &)));
    TEST_MEMBER_RETURN(UM const &, S, count(D(K const &)));
    TEST_MEMBER_RETURN(UM &, ITP, equal_range(D(K const &)));
    TEST_MEMBER_RETURN(UM const &, CITP, equal_range(D(K const &)));
    TEST_MEMBER_RETURN(UM &, M &, operator[](D(K const &)));
    TEST_MEMBER_RETURN(UM &, M &, operator[](D(K &&)));
    TEST_MEMBER_RETURN(UM &, M &, at(D(K const &)));
    TEST_MEMBER_RETURN(UM const &, M const &, at(D(K const &)));


    // Bucket interface:

    TEST_MEMBER_RETURN(UM const &, S, bucket_count());
    TEST_MEMBER_RETURN(UM const &, S, max_bucket_count());
    TEST_MEMBER_RETURN(UM const &, S, bucket_size(D(S)));
    TEST_MEMBER_RETURN(UM const &, S, bucket(D(K const &)));

    TEST_MEMBER_RETURN(UM &, LIT, begin(D(S)));
    TEST_MEMBER_RETURN(UM const &, CLIT, begin(D(S)));
    TEST_MEMBER_RETURN(UM &, LIT, end(D(S)));
    TEST_MEMBER_RETURN(UM const &, CLIT, end(D(S)));
    TEST_MEMBER_RETURN(UM &, CLIT, cbegin(D(S)));
    TEST_MEMBER_RETURN(UM const &, CLIT, cbegin(D(S)));
    TEST_MEMBER_RETURN(UM &, CLIT, cend(D(S)));
    TEST_MEMBER_RETURN(UM const &, CLIT, cend(D(S)));


    // Hash policy:

    TEST_MEMBER_RETURN(UM const &, float, load_factor());
    TEST_MEMBER_RETURN(UM const &, float, max_load_factor());
    TEST_MEMBER_RETURN(UM &, void, max_load_factor(D(float)));
    TEST_MEMBER_RETURN(UM &, void, rehash(D(S)));
    TEST_MEMBER_RETURN(UM &, void, reserve(D(S)));
};

template <typename ... Args>
struct Types {
    using UM = typename Type<sharemind::UnorderedMap<Args...> >::type;
    using RUM = typename Type<std::unordered_map<Args...> >::type;
    TEST_SAME_FIELD(key_type);
    TEST_SAME_FIELD(value_type);
    TEST_SAME_FIELD(mapped_type);
    TEST_SAME_FIELD(hasher);
    TEST_SAME_FIELD(key_equal);
    TEST_SAME_FIELD(allocator_type);
    TEST_SAME_FIELD(pointer);
    TEST_SAME_FIELD(const_pointer);
    TEST_SAME_FIELD(reference);
    TEST_SAME_FIELD(const_reference);
    TEST_SAME_FIELD(size_type);
    TEST_SAME_FIELD(difference_type);
    TEST_IT_TRAIT_EQ(iterator);
    TEST_IT_TRAIT_EQ(const_iterator);
    TEST_IT_TRAIT_EQ(local_iterator);
    TEST_IT_TRAIT_EQ(const_local_iterator);
    using TUM = Type<UM>;
    using TRUM = Type<RUM>;
};

template <typename ... Args>
using UM = typename Types<Args...>::UM;

template <typename ... Args>
using RUM = typename Types<Args...>::RUM;

template <template <typename ... Args> class UM>
void testUnorderedMap() {
    {
        UM<unsigned, std::string> m;
        SHAREMIND_TESTASSERT(m.empty());

        std::unordered_set<unsigned> usedKeys;

        typename decltype(m)::key_type lastKey;
        for (std::size_t size = 0u; size < ITERATIONS; ++size) {
            SHAREMIND_TESTASSERT(m.size() == size);
            do {
                lastKey = myRand->get();
            } while (usedKeys.find(lastKey) != usedKeys.end());
            usedKeys.emplace(lastKey);

            auto const r(m.emplace(lastKey, std::to_string(lastKey)));
            SHAREMIND_TESTASSERT(r.second);
            SHAREMIND_TESTASSERT(r.first->first == lastKey);
            SHAREMIND_TESTASSERT(r.first->second == std::to_string(lastKey));
            SHAREMIND_TESTASSERT(!m.empty());
            SHAREMIND_TESTASSERT(m.size() == size + 1u);

            for (auto it = m.cbegin(); it != m.cend(); ++it) {
                for (unsigned const key : usedKeys) {
                    auto const r2(m.emplace_hint(it, key, "FAIL"));
                    SHAREMIND_TESTASSERT(r2->first == key);
                    SHAREMIND_TESTASSERT(r2->second == std::to_string(key));
                    SHAREMIND_TESTASSERT(!m.empty());
                    SHAREMIND_TESTASSERT(m.size() == size + 1u);

                    auto const r3(m.emplace(key, "FAIL"));
                    SHAREMIND_TESTASSERT(!r3.second);
                    SHAREMIND_TESTASSERT(r3.first->first == key);
                    SHAREMIND_TESTASSERT(r3.first->second == std::to_string(key));
                    SHAREMIND_TESTASSERT(!m.empty());
                    SHAREMIND_TESTASSERT(m.size() == size + 1u);
                }
            }
        }
        for (auto const key : usedKeys) {
            auto const r(m.find(key));
            SHAREMIND_TESTASSERT(r != m.end());
            SHAREMIND_TESTASSERT(r->first == key);
            SHAREMIND_TESTASSERT(r->second == std::to_string(key));
        }
        for (auto const key : usedKeys) {
            auto const r(const_cast<decltype(m) const &>(m).find(key));
            SHAREMIND_TESTASSERT(r != m.cend());
            SHAREMIND_TESTASSERT(r->first == key);
            SHAREMIND_TESTASSERT(r->second == std::to_string(key));
        }
        {
            decltype(usedKeys) keys2;
            for (auto const & vp : m) {
                SHAREMIND_TESTASSERT(usedKeys.find(vp.first) != usedKeys.end());
                keys2.emplace(vp.first);
                SHAREMIND_TESTASSERT(vp.second == std::to_string(vp.first));
            }
            SHAREMIND_TESTASSERT(usedKeys == keys2);
        }
        {
            decltype(usedKeys) keys2;
            for (auto const & vp : const_cast<decltype(m) const &>(m)) {
                SHAREMIND_TESTASSERT(usedKeys.find(vp.first) != usedKeys.end());
                keys2.emplace(vp.first);
                SHAREMIND_TESTASSERT(vp.second == std::to_string(vp.first));
            }
            SHAREMIND_TESTASSERT(usedKeys == keys2);
        }
        {
            auto copy1(m);
            SHAREMIND_TESTASSERT(m == copy1);
            auto copy2(m);
            SHAREMIND_TESTASSERT(m == copy2);
            SHAREMIND_TESTASSERT(copy1 == copy2);
            auto copy3(m);
            SHAREMIND_TESTASSERT(m == copy3);
            SHAREMIND_TESTASSERT(copy1 == copy2);
            SHAREMIND_TESTASSERT(copy1 == copy3);
        }
        {
            static_assert(ITERATIONS > 4u, "");
            SHAREMIND_TESTASSERT(m.size() == ITERATIONS);
            auto copy(m);
            auto const original(copy);
            auto eraseFrom(copy.begin());
            std::advance(eraseFrom, ITERATIONS / 4u);
            auto eraseTo(eraseFrom);
            std::advance(eraseTo, ITERATIONS / 2u);
            decltype(copy) erasedAndLeftValues(eraseFrom, eraseTo);
            copy.erase(eraseFrom, eraseTo);
            SHAREMIND_TESTASSERT(copy.size() == m.size() - (ITERATIONS / 2u));
            erasedAndLeftValues.insert(copy.begin(), copy.end());
            SHAREMIND_TESTASSERT(erasedAndLeftValues == original);
        }
        for (auto const & key : usedKeys)
            SHAREMIND_TESTASSERT(m.erase(key) == 1u);
        SHAREMIND_TESTASSERT(m.size() == 0u);
        SHAREMIND_TESTASSERT(m.empty());
    }

    {
        UM<unsigned, std::string> m;
        m.emplace(32, "asdf");
        SHAREMIND_TESTASSERT(m.find(32u) == m.begin());
        SHAREMIND_TESTASSERT(m.find(32u)->first == 32u);
        SHAREMIND_TESTASSERT(m.find(32u)->second == "asdf");
        auto er42(m.equal_range(42u));
        SHAREMIND_TESTASSERT(er42.first == er42.second);
        SHAREMIND_TESTASSERT(er42.first == m.end());
        SHAREMIND_TESTASSERT(er42.second == m.end());
        auto er32(m.equal_range(32u));
        SHAREMIND_TESTASSERT(er32.first != er32.second);
        SHAREMIND_TESTASSERT(er32.first == m.begin());
        SHAREMIND_TESTASSERT(er32.second == m.end());

        SHAREMIND_TESTASSERT(m[32u] == "asdf");
        SHAREMIND_TESTASSERT(m.at(32u) == "asdf");
        try {
            m.at(42u);
            SHAREMIND_TEST_UNREACHABLE;
        } catch (std::out_of_range const & e) {
            SHAREMIND_TESTASSERT(std::strlen(e.what()) > 0u);
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }
}

struct MyHash {
    std::size_t operator()(std::string const & v) const noexcept
    { return std::hash<std::string>()(v) + 10u; }

    using result_type = decltype(std::hash<std::string>()(""));
};

struct MyKeyEqual {
    static char const * ensureCString(std::string const & str) noexcept
    { return str.c_str(); }

    static char const * ensureCString(char const * const str) noexcept
    { return str; }

    template <typename A, typename B>
    bool operator()(A && a, B && b) const noexcept {
        return std::strcmp(ensureCString(std::forward<A>(a)),
                           ensureCString(std::forward<B>(b))) == 0;
    }
};

} // anonymous namespace

int main() {
    myRand = new Rand;
    try {
        testUnorderedMap<RUM>();
        testUnorderedMap<UM>();

        {
            char const cs[] = "teretere";
            std::string const s(cs);

            {
                sharemind::UnorderedMap<std::string, int, MyHash> m;
                auto const & cm = m;
                m.emplace(s, 42);
                auto const it(m.find(s));
                SHAREMIND_TESTASSERT(it != m.end());
                SHAREMIND_TESTASSERT(it->first == s);
                SHAREMIND_TESTASSERT(it->second == 42);
                {
                    auto const it2(cm.find(cs));
                    SHAREMIND_TESTASSERT(it2 != cm.end());
                    SHAREMIND_TESTASSERT(it2->first == s);
                    SHAREMIND_TESTASSERT(it2->second == 42);
                }{
                    auto const it2(m.find(cs));
                    SHAREMIND_TESTASSERT(it2 != m.end());
                    SHAREMIND_TESTASSERT(it2->first == s);
                    SHAREMIND_TESTASSERT(it2->second == 42);
                }{
                    auto const it2(cm.find(cs));
                    SHAREMIND_TESTASSERT(it2 != cm.end());
                    SHAREMIND_TESTASSERT(it2->first == s);
                    SHAREMIND_TESTASSERT(it2->second == 42);
                }

                auto const hash(MyHash()(s));
                SHAREMIND_TESTASSERT(m.find(hash, s) == it);
                SHAREMIND_TESTASSERT(cm.find(hash, s) == it);
                SHAREMIND_TESTASSERT(m.find(hash, MyKeyEqual(), s) == it);
                SHAREMIND_TESTASSERT(cm.find(hash, MyKeyEqual(), s) == it);
                SHAREMIND_TESTASSERT(m.find(hash, cs) == it);
                SHAREMIND_TESTASSERT(cm.find(hash, cs) == it);
                SHAREMIND_TESTASSERT(m.find(hash, MyKeyEqual(), cs) == it);
                SHAREMIND_TESTASSERT(cm.find(hash, MyKeyEqual(), cs) == it);
            }{
                sharemind::UnorderedMap<std::string,
                                        int,
                                        sharemind::StringHasher> m;
                auto const & cm = m;
                m.emplace(s, 42);
                auto const it(m.find(s));
                SHAREMIND_TESTASSERT(it != m.end());
                SHAREMIND_TESTASSERT(it->first == s);
                SHAREMIND_TESTASSERT(it->second == 42);

                using sharemind::getOrCreateTemporaryStringHashTablePredicate;
                auto const p(getOrCreateTemporaryStringHashTablePredicate(s));
                SHAREMIND_TESTASSERT(m.find(p) == it);
                SHAREMIND_TESTASSERT(m.find(p.hash(), p) == it);

                SHAREMIND_TESTASSERT(cm.find(p) == it);
                SHAREMIND_TESTASSERT(cm.find(p.hash(), p) == it);
            }
        }

        {
            sharemind::UnorderedMap<std::string, std::size_t, MyHash> m;
            for (std::size_t i = 0u; i < ITERATIONS; ++i) {
                auto const r(m.emplace(std::to_string(i) + "haha", i));
                auto const & key = r.first->first;
                auto bucket(m.bucket(key));
                SHAREMIND_TESTASSERT(m.hash_bucket(MyHash()(key)) == bucket);
                SHAREMIND_TESTASSERT(m.bucket(r.first) == bucket);
            }
            for (auto it = m.begin(); it != m.end(); ++it) {
                auto const & key = it->first;
                auto bucket(m.bucket(key));
                SHAREMIND_TESTASSERT(m.hash_bucket(MyHash()(key)) == bucket);
                SHAREMIND_TESTASSERT(m.bucket(it) == bucket);
            }
        }
    } catch (...) {
        delete myRand;
        throw;
    }
    delete myRand;
}
