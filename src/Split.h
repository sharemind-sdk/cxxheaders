/*
 * Copyright (C) 2017 Cybernetica
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

#ifndef SHAREMIND_SPLIT_H
#define SHAREMIND_SPLIT_H

namespace sharemind {

template <typename InputContainer,
          typename DelimPredicate,
          typename MatchAction>
inline void split(InputContainer const & s,
                  DelimPredicate delimPredicate,
                  MatchAction matchAction)
{
    auto last = s.cbegin();
    decltype(last) it;
    for (; last != s.cend(); (matchAction(last, it), last = ++it)) {
        it = last;
        while (!delimPredicate(*it)) {
            if (++it == s.cend()) {
                matchAction(last, it);
                return;
            }
        }
    }
}

template <typename InputContainer,
          typename DelimPredicate,
          typename MatchAction>
inline void splitNoAllowEmpty(InputContainer const & s,
                              DelimPredicate delimPredicate,
                              MatchAction matchAction)
{
    auto last = s.cbegin();
    decltype(last) it;
    for (;; (matchAction(last, it), last = ++it)) {
        for (;; ++last) { // Skip delimeters
            if (last == s.cend())
                return;
            if (!delimPredicate(*last))
                break;
        }; // last now points to first non-delimeter
        it = last;
        do {
            if (++it == s.cend()) {
                matchAction(last, it);
                return;
            }
        } while (!delimPredicate(*it));
    }
}

} /* namespace sharemind */

#endif /* SHAREMIND_SPLIT_H */
