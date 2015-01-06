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

#ifndef SHAREMIND_GCCPR61067_H
#define SHAREMIND_GCCPR61067_H

#include "GccVersion.h"


/*
  This is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61067
  which prevents code like the following to compile with GCC before 4.8.

    struct Base { virtual ~Base() noexcept; };
    struct A { ~A(); };
    struct Derived: Base { A a; };

  Such code resulted in the following error with GCC 4.7:

    test.cpp:3:8: error: looser throw specifier for 'virtual Derived::~Derived()'
    test.cpp:1:23: error:   overriding 'virtual Base::~Base() noexcept (true)'

  Given that the destructor for struct A can't be changed, use this workaround
  as follows, by means of wrapping the base type:

    struct Base { virtual ~Base() noexcept; };
    struct A { ~A(); };
    struct Derived: SHAREMIND_GCCPR61067_WRAPPED(Base) { A a; };

*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
#include <utility>
#include "GccInheritContructor.h"
namespace sharemind {
namespace workaround {

template <typename T>
class GCC_PR61067_Wrapper: public T {

public: /* Methods: */

    SHAREMIND_GCC_INHERITED_CONSTRUCTOR_WORKAROUND(GCC_PR61067_Wrapper,T,T)

    using T::operator=;

    inline ~GCC_PR61067_Wrapper() noexcept {}

}; // class Wrapper {

} /* namespace workaround { */
} // namespace sharemind {
#define SHAREMIND_GCCPR61067_WRAPPED(...) \
    sharemind::workaround::GCC_PR61067_Wrapper<__VA_ARGS__>
#else
#define SHAREMIND_GCCPR61067_WRAPPED(...) __VA_ARGS__
#endif

#endif // SHAREMIND_GCCPR61067_H
