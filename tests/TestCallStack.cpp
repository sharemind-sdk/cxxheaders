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

#include "../src/CallStack.h"

#include <type_traits>
#include "../src/TestAssert.h"


using sharemind::CallStack;

namespace {

struct Key { unsigned value; };
struct Value { unsigned value; };

bool operator==(Key const lhs, Key const rhs) noexcept
{ return lhs.value == rhs.value; }

#define SA(...) static_assert(__VA_ARGS__, "")
SA(std::is_same<typename CallStack<Key>::KeyType, Key>::value);
SA(std::is_same<typename CallStack<Key>::Context::KeyType, Key>::value);
SA(std::is_same<typename CallStack<Key, Value>::KeyType, Key>::value);
SA(std::is_same<typename CallStack<Key, Value>::Context::KeyType, Key>::value);
SA(std::is_same<typename CallStack<Key, Value>::ValueType, Value>::value);
SA(std::is_same<typename CallStack<Key, Value>::Context::ValueType, Value>::value);

constexpr unsigned const maxLevels = 24u;

void testWithoutValue(unsigned level = 0u) {
    using CS = CallStack<Key>;
    auto oldTop = CS::top();
    if (level > 0u) {
        auto context = oldTop;
        SHAREMIND_TESTASSERT(context != nullptr);
        for (unsigned expected = level - 1u;; --expected) {
            SHAREMIND_TESTASSERT(context->key().value == expected);
            SHAREMIND_TESTASSERT(CS::contains(Key{expected}) == context);
            {
                CS::Context const overrideContext(Key{expected});
                SHAREMIND_TESTASSERT(CS::top() == &overrideContext);
                SHAREMIND_TESTASSERT(CS::contains(Key{expected})
                                     == &overrideContext);
            }
            SHAREMIND_TESTASSERT(CS::top() == oldTop);
            context = context->next();
            if (expected == 0u)
                break;
        }
        SHAREMIND_TESTASSERT(context == nullptr);
    }
    SHAREMIND_TESTASSERT(CS::contains(Key{level}) == nullptr);
    CS::Context const context(Key{level});
    SHAREMIND_TESTASSERT(context.key().value == level);
    SHAREMIND_TESTASSERT(context.next() == oldTop);
    SHAREMIND_TESTASSERT(CS::contains(Key{level}) == &context);
    SHAREMIND_TESTASSERT(CS::top() == &context);
    if (level < maxLevels)
        testWithoutValue(level + 1u);
}

void testWithValue(unsigned level = 0u) {
    using CS = CallStack<Key, Value>;
    auto oldTop = CS::top();
    if (level > 0u) {
        auto context = oldTop;
        SHAREMIND_TESTASSERT(context != nullptr);
        for (unsigned expected = level - 1u;; --expected) {
            SHAREMIND_TESTASSERT(context->key().value == expected);
            SHAREMIND_TESTASSERT(context->value().value == expected + 42u);
            SHAREMIND_TESTASSERT(CS::contains(Key{expected}) == context);
            {
                CS::Context const overrideContext(Key{expected}, Value{});
                SHAREMIND_TESTASSERT(CS::top() == &overrideContext);
                SHAREMIND_TESTASSERT(CS::contains(Key{expected})
                                     == &overrideContext);
            }
            SHAREMIND_TESTASSERT(CS::top() == oldTop);
            context = context->next();
            if (expected == 0u)
                break;
        }
        SHAREMIND_TESTASSERT(context == nullptr);
    }
    SHAREMIND_TESTASSERT(CS::contains(Key{level}) == nullptr);
    CS::Context const context(Key{level}, Value{level + 42u});
    SHAREMIND_TESTASSERT(context.key().value == level);
    SHAREMIND_TESTASSERT(context.value().value == level + 42u);
    SHAREMIND_TESTASSERT(context.next() == oldTop);
    SHAREMIND_TESTASSERT(CS::contains(Key{level}) == &context);
    SHAREMIND_TESTASSERT(CS::top() == &context);
    if (level < maxLevels)
        testWithValue(level + 1u);
}

} // anonymous namespace

int main() {
    testWithoutValue();
    testWithValue();
}
