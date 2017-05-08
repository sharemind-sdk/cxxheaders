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

template <typename InputIterator,
          typename DelimPredicate,
          typename MatchAction>
inline void split(InputIterator first,
                  InputIterator last,
                  DelimPredicate delimPredicate,
                  MatchAction matchAction)
{
    auto lastIt = first;
    decltype(lastIt) it;
    for (; lastIt != last; (matchAction(lastIt, it), lastIt = ++it)) {
        it = lastIt;
        while (!delimPredicate(*it)) {
            if (++it == last) {
                matchAction(lastIt, it);
                return;
            }
        }
    }
}

template <typename InputIterator,
          typename DelimPredicate,
          typename MatchAction>
inline void splitNoAllowEmpty(InputIterator first,
                              InputIterator last,
                              DelimPredicate delimPredicate,
                              MatchAction matchAction)
{
    auto lastIt = first;
    decltype(lastIt) it;
    for (;; (matchAction(lastIt, it), lastIt = ++it)) {
        for (;; ++lastIt) { // Skip delimeters
            if (lastIt == last)
                return;
            if (!delimPredicate(*lastIt))
                break;
        }; // lastIt now points to first non-delimeter
        it = lastIt;
        do {
            if (++it == last) {
                matchAction(lastIt, it);
                return;
            }
        } while (!delimPredicate(*it));
    }
}

} /* namespace sharemind */

#endif /* SHAREMIND_SPLIT_H */
