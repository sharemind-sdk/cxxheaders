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

#ifndef SHAREMIND_SPINWAIT_H
#define SHAREMIND_SPINWAIT_H

namespace sharemind {

inline void spinWait() noexcept {
    #if defined(__i386__) || defined(__x86_64__) \
        || defined(__amd64__) || defined(__ia64__)
    __asm__ __volatile__ ("pause");
    #endif
}

} /* namespace sharemind { */

#endif /* SHAREMIND_SPINWAIT_H */
