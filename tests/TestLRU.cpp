/*
 * Copyright (C) 2016 Cybernetica
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

#include <memory>
#include <string>
#include "../src/LRU.h"

struct Elem {};

int main() {
    using sharemind::LRU;

    LRU<std::string, Elem> lru{1};
    auto elem1 = std::make_shared<Elem>();
    auto elem2 = std::make_shared<Elem>();

    lru.insert("key1", elem1);
    // elem1 in cache and here
    assert(elem1.use_count() == 2);
    assert(lru.get("key1") == elem1);
    lru.insert("key2", elem2);
    assert(elem2.use_count() == 2);
    // elem1 only in here (weak in cache)
    assert(elem1.use_count() == 1);
    // get a weak element
    assert(lru.get("key1") == elem1);
    // elem1 was used, so now also stored in cache
    assert(elem1.use_count() == 2);
    assert(elem2.use_count() == 1);
    elem2.reset();
    // get a expired weak element
    assert(lru.get("key2") == nullptr);
    elem2.reset(new Elem{});
    // overwrite elem
    lru.insert("key1", elem2);
    assert(elem1.use_count() == 1);
    lru.insert("key2", elem2);
    assert(elem2.use_count() == 2);
    // overwrite weak element
    lru.insert("key1", elem2);
    assert(elem2.use_count() == 2);
    lru.clear();
    assert(elem1.use_count() == 1);
    assert(elem2.use_count() == 1);
}
