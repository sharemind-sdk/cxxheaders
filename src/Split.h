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
    InputIterator it;
    for (; first != last; first = ++it) {
        it = first;
        while (!delimPredicate(*it)) {
            if (++it == last) {
                matchAction(first, it);
                return;
            }
        }
        matchAction(first, it);
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
    InputIterator it;
    for (;; first = ++it) {
        for (;; ++first) { // Skip delimeters
            if (first == last)
                return;
            if (!delimPredicate(*first))
                break;
        }; // first now points to the first non-delimeter in a sequence
        it = first;
        do {
            if (++it == last) {
                matchAction(first, it);
                return;
            }
        } while (!delimPredicate(*it));
        matchAction(first, it);
    }
}

} /* namespace sharemind */

#endif /* SHAREMIND_SPLIT_H */
