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

#include "../src/IsSwappable.h"


namespace S = sharemind;

struct NotSwappableBase {
    NotSwappableBase(NotSwappableBase&&) = delete;
    NotSwappableBase & operator=(NotSwappableBase &&) = delete;
};
#define C(name) struct name: NotSwappableBase {}

#define CASE_WITH_ONE(name, r1, r2, r3, r4, ...) \
    namespace name { \
    namespace Test { __VA_ARGS__ } \
    static_assert(S::IsSwappable<Test::A>::value == r1, ""); \
    static_assert(S::IsNothrowSwappable<Test::A>::value == r2, ""); \
    static_assert(S::IsSwappableWith<Test::A &, Test::A &>::value == r3, ""); \
    static_assert(S::IsNothrowSwappableWith<Test::A &, Test::A &>::value == r4,\
                  ""); \
    } /* namespace name */

#define SW(a,b) void swap(a &, b &)

CASE_WITH_ONE(Case1, false, false, false, false, C(A); )
CASE_WITH_ONE(Case2, true,  false, true,  false, C(A); SW(A,A);)
CASE_WITH_ONE(Case3, true,  true,  true,  true,  C(A); SW(A,A) noexcept;)

#define C2(name,mc,ma) \
    struct name { \
        name(name &&) mc; \
        name & operator=(name &&) ma; \
    }

CASE_WITH_ONE(Case4,  false, false, false, false, C2(A, = delete, = delete);)
CASE_WITH_ONE(Case5,  false, false, false, false, C2(A, = delete,);)
CASE_WITH_ONE(Case6,  false, false, false, false, C2(A, = delete, noexcept);)
CASE_WITH_ONE(Case7,  false, false, false, false, C2(A,, = delete);)
CASE_WITH_ONE(Case8,  true,  false, true,  false, C2(A,,);)
CASE_WITH_ONE(Case9,  true,  false, true,  false, C2(A,, noexcept);)
CASE_WITH_ONE(Case10, false, false, false, false, C2(A, noexcept, = delete);)
CASE_WITH_ONE(Case11, true,  false, true,  false, C2(A, noexcept,);)
CASE_WITH_ONE(Case12, true,  true,  true,  true,  C2(A, noexcept, noexcept);)


#define CASE_WITH_TWO(name, r1, r2, ...) \
    namespace name { \
    namespace Test { C(A); C(B); __VA_ARGS__ } \
    static_assert(S::IsSwappableWith<Test::A &, Test::B &>::value == r1, ""); \
    static_assert(S::IsNothrowSwappableWith<Test::A &, Test::B &>::value == r2,\
                  ""); \
    } /* namespace name */

CASE_WITH_TWO(TwoCase1, false, false,);
CASE_WITH_TWO(TwoCase2, false, false, SW(A,B););
CASE_WITH_TWO(TwoCase3, false, false, SW(A,B) noexcept;);
CASE_WITH_TWO(TwoCase4, false, false,                   SW(B,A););
CASE_WITH_TWO(TwoCase5, true,  false, SW(A,B);          SW(B,A););
CASE_WITH_TWO(TwoCase6, true,  false, SW(A,B) noexcept; SW(B,A););
CASE_WITH_TWO(TwoCase7, false, false,                   SW(B,A) noexcept;);
CASE_WITH_TWO(TwoCase8, true,  false, SW(A,B);          SW(B,A) noexcept;);
CASE_WITH_TWO(TwoCase9, true,  true,  SW(A,B) noexcept; SW(B,A) noexcept;);

int main() {}
