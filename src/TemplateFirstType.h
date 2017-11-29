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

#ifndef SHAREMIND_TEMPLATEFIRSTTYPE_H
#define SHAREMIND_TEMPLATEFIRSTTYPE_H

namespace sharemind {

/*
  TemplateFirstType was previously defined as

    template <typename T, typename ...> using TemplateFirstType = T;

  But this caused problems with some compilers, e.g. "pack expansion argument
  for non-pack parameter" errors. To work around these issues we needed to add
  another level of indirection. See GCC PR 59498 [1] and C++ DR 1430 [2] for
  details.

  [1]: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59498
  [2]: http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
*/

namespace Detail {
namespace TemplateFirstType {

template <typename T, typename ...> struct Impl { using type = T; };

} /* namespace TemplateFirstType { */
} /* namespace Detail { */

template <typename ... Ts>
using TemplateFirstType = typename Detail::TemplateFirstType::Impl<Ts...>::type;

} /* namespace sharemind { */

#endif /* SHAREMIND_TEMPLATEFIRSTTYPE_H */
