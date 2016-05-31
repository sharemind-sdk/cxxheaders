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

#ifndef SHAREMIND_MPSC_WAITFREE_SEMIINTRUSIVE_QUEUE_H
#define SHAREMIND_MPSC_WAITFREE_SEMIINTRUSIVE_QUEUE_H

#include <atomic>
#include <cassert>
#include <cstddef>
#include <memory>
#include <sharemind/AlignToCacheLine.h>
#include <sharemind/compiler-support/GccVersion.h>
#include <type_traits>
#include <utility>
#include "AlignedAllocator.h"


namespace sharemind {

template <typename T>
class MpscWaitFreeSemiIntrusiveQueue {

    /* The following fails in std::is_nothrow_move_assignable with GCC older
       than version 5 for unknown reasons if T is an incomplete type. A
       contained testcase of the same error is here:

        template <typename T> struct I {};
        template <typename T> struct Q { using X = decltype(I<T>() = I<T>()); };
        template <typename T> struct X;
        template <typename T> using MQ = Q<X<T> >;
        template <typename T> struct X { using N = typename MQ<T>::X; };
        int main() { MQ<int>(); }
    */
    #if !defined(SHAREMIND_GCC_VERSION) || (SHAREMIND_GCC_VERSION > 50000)
    static_assert(std::is_nothrow_move_assignable<T>::value,
                  "T is required to be noexcept move assignable!");
    #endif

public: /* Types: */

    struct SHAREMIND_ALIGN_TO_CACHE_SIZE Node {

        inline Node(Node &&) = default;
        inline Node(Node const &) = default;

        template <typename ... Args>
        inline Node(Args && ... args)
            : next{nullptr}
            , data{std::forward<Args>(args)...}
        {}

        SHAREMIND_ALIGNEDALLOCATION_MEMBERS(alignof(Node))

        SHAREMIND_ALIGN_TO_CACHE_SIZE std::atomic<Node *> next;
        T data;

    };

public: /* Methods: */

    MpscWaitFreeSemiIntrusiveQueue(MpscWaitFreeSemiIntrusiveQueue const &)
            = delete;
    MpscWaitFreeSemiIntrusiveQueue & operator=(
            MpscWaitFreeSemiIntrusiveQueue const &) = delete;

    inline MpscWaitFreeSemiIntrusiveQueue()
        : MpscWaitFreeSemiIntrusiveQueue{new Node{}}
    {}

    inline ~MpscWaitFreeSemiIntrusiveQueue() noexcept {
        while (pop());
        delete m_head.load(std::memory_order_relaxed);
    }

    SHAREMIND_ALIGNEDALLOCATION_MEMBERS(alignof(MpscWaitFreeSemiIntrusiveQueue))

    inline void push(std::unique_ptr<Node> node) noexcept {
        assert(node);
        Node * const newNode = node.release();
        assert(!newNode->next.load(std::memory_order_relaxed));
        Node * const oldTail =
                m_tail.exchange(newNode, std::memory_order_relaxed);
        assert(oldTail);
        oldTail->next.store(newNode, std::memory_order_relaxed);
    }

    inline std::unique_ptr<Node> pop() noexcept {
        Node * const head = m_head.load(std::memory_order_consume);
        assert(head);
        Node * const next = head->next.load(std::memory_order_relaxed);

        if (!next)
            return nullptr;

        head->data = std::move(next->data);
        m_head.store(next, std::memory_order_release);
        head->next.store(nullptr, std::memory_order_relaxed);
        return std::unique_ptr<Node>{head};
    }

    inline bool empty() noexcept {
        Node * const head = m_head.load(std::memory_order_relaxed);
        assert(head);
        return !head->next.load(std::memory_order_relaxed);
    }

private: /* Methods: */

    inline MpscWaitFreeSemiIntrusiveQueue(Node * const node)
        : m_tail{node}
        , m_head{node}
    {}

private: /* Fields: */

    SHAREMIND_ALIGN_TO_CACHE_SIZE std::atomic<Node *> m_tail;
    SHAREMIND_ALIGN_TO_CACHE_SIZE std::atomic<Node *> m_head;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_MPSC_WAITFREE_SEMIINTRUSIVE_QUEUE_H */
