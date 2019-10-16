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

#ifndef SHAREMIND_CALLSTACK_H
#define SHAREMIND_CALLSTACK_H

#include <type_traits>
#include <utility>


namespace sharemind {

template <typename Key, typename Value = void>
class CallStack {

public: /* Types: */

    using KeyType = Key;
    using ValueType = Value;

    class Context {

        friend class CallStack;

    public: /* Types: */

        using KeyType = Key;
        using ValueType = Value;

    public: /* Methods: */

        Context(Context &&) = delete;
        Context(Context const &) = delete;

        template <typename Key_>
        explicit Context(Key_ && key)
                noexcept(std::is_nothrow_constructible<Key, Key_ &&>::value
                         && std::is_nothrow_default_constructible<Value>::value)
            : m_key(std::forward<Key_>(key))
        { CallStack::m_topContext = this; }

        template <typename Key_, typename Value_>
        Context(Key_ && key, Value_ && value)
                noexcept(std::is_nothrow_constructible<Key, Key_ &&>::value
                         && std::is_nothrow_constructible<Value,
                                                          Value_ &&>::value)
            : m_key(std::forward<Key_>(key))
            , m_value(std::forward<Value_>(value))
        { CallStack::m_topContext = this; }


        ~Context() noexcept { CallStack::m_topContext = m_next; }


        Context & operator=(Context &&) = delete;
        Context & operator=(Context const &) = delete;


        Key & key() noexcept { return m_key; }
        Key const & key() const noexcept { return m_key; }

        Value & value() noexcept { return m_value; }
        Value const & value() const noexcept { return m_value; }

        Context * next() const noexcept { return m_next; }

    private: /* Fields: */

        Key m_key;
        Value m_value;
        Context * const m_next = CallStack::m_topContext;

    };
    friend class Context;

public: /* Methods: */

    static Context * top() noexcept { return m_topContext; }

    template <typename Key_>
    static Context * contains(Key_ && key) noexcept {
        for (auto c = m_topContext; c != nullptr; c = c->m_next)
            if (c->m_key == key)
                return c;
        return nullptr;
    }

private: /* Fields: */

    static thread_local Context * m_topContext;

};

template <typename Key, typename Value>
thread_local typename CallStack<Key, Value>::Context *
CallStack<Key, Value>::m_topContext = nullptr;

template <typename Key>
class CallStack<Key, void> {

public: /* Types: */

    using KeyType = Key;

    class Context {

        friend class CallStack;

    public: /* Types: */

        using KeyType = Key;

    public: /* Methods: */

        Context(Context &&) = delete;
        Context(Context const &) = delete;

        template <typename Key_>
        explicit Context(Key_ && key)
                noexcept(std::is_nothrow_constructible<Key, Key_ &&>::value)
            : m_key(std::forward<Key_>(key))
        { CallStack::m_topContext = this; }


        ~Context() noexcept { CallStack::m_topContext = m_next; }


        Context & operator=(Context &&) = delete;
        Context & operator=(Context const &) = delete;


        Key & key() noexcept { return m_key; }
        Key const & key() const noexcept { return m_key; }

        Context * next() const noexcept { return m_next; }

    private: /* Fields: */

        Key m_key;
        Context * const m_next = CallStack::m_topContext;

    };
    friend class Context;

public: /* Methods: */

    static Context * top() noexcept { return m_topContext; }

    template <typename Key_>
    static Context * contains(Key_ && key) noexcept {
        for (auto c = m_topContext; c != nullptr; c = c->m_next)
            if (c->m_key == key)
                return c;
        return nullptr;
    }

private: /* Fields: */

    static thread_local Context * m_topContext;

};

template <typename Key>
thread_local typename CallStack<Key, void>::Context *
CallStack<Key, void>::m_topContext = nullptr;

} /* namespace Sharemind { */

#endif /* SHAREMIND_CALLSTACK_H */
