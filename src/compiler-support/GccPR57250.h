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

#ifndef SHAREMIND_GCCPR57250_H
#define SHAREMIND_GCCPR57250_H

#include <sharemind/compiler-support/GccVersion.h>


/*
  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=57250 std::atomic support for
  std::shared_ptr was not implemented in GCC versions before 5.0. Currently no
  good workaround.
*/

#if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 50000)
#define SHAREMIND_GCCPR57250 true
#else
#define SHAREMIND_GCCPR57250 false
#endif

#endif /* SHAREMIND_GCCPR57250_H */
