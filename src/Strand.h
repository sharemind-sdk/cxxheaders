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

#ifndef SHAREMIND_STRAND_H
#define SHAREMIND_STRAND_H

#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <utility>
#include "CallStack.h"
#include "StrongType.h"
#include "ThreadPool.h"

#include <thread>
#include <type_traits>
#include "TicketSpinLock.h"


namespace sharemind {

/**
  \brief Allows queueing of serial jobs on any thread pool.

  This class provides means to utilize a thread pool to execute jobs which
  are not allowed to run in parallel. Even if the thread pool has N > 1
  threads, no two jobs queued by this class will be executed in parallel.
*/
class Strand {

private: /* Types: */

    struct Internal {

    /* Types: */

        using CallStackRecursionIndicator =
                StrongType<
                    Internal *,
                    struct InternalCallStackRecursionIndicator_Tag,
                    StrongTypeEqualityComparable
                >;

    /* Methods: */

        Internal(std::shared_ptr<ThreadPool> threadPool)
            : m_threadPool(std::move(threadPool))
        {}

        void submit(ThreadPool::Task && task) noexcept {
            assert(task);
            assert(task->m_value);
            assert(!task->m_next);
            ThreadPool::TaskWrapper * const newTail = task.get();
            std::lock_guard<decltype(m_tailMutex)> const guard(m_tailMutex);
            ThreadPool::TaskWrapper * const oldTail = m_tail;
            oldTail->m_value = std::move(task->m_value);
            oldTail->m_next = std::move(task);
            m_tail = newTail;

            if (m_sliceTask && m_threadPool)
                m_threadPool->submit(std::move(m_sliceTask));
        }

        std::shared_ptr<ThreadPool> stopAndJoin() noexcept {
            std::shared_ptr<ThreadPool> gcThreadPool;
            std::unique_lock<decltype(m_tailMutex)> tailLock(m_tailMutex);
            gcThreadPool = std::move(m_threadPool);
            assert(!m_threadPool);
            m_joinCond.wait(tailLock,
                            [this]() noexcept { return m_sliceTask.get(); });
            return gcThreadPool;
        }

        std::shared_ptr<ThreadPool> stopAndMaybeJoin() noexcept {
            if (!CallStack<CallStackRecursionIndicator>::contains(
                    CallStackRecursionIndicator(this)))
                return stopAndJoin();
            std::shared_ptr<ThreadPool> gcThreadPool;
            {
                std::lock_guard<decltype(m_tailMutex)> const tailGuard(
                            m_tailMutex);
                gcThreadPool = std::move(m_threadPool);
            }
            return gcThreadPool;
        }

        void run(ThreadPool::Task && sliceTask) noexcept {
            CallStack<CallStackRecursionIndicator>::Context context(this);
            {
                // Retrieve first task (or return if stopping):
                ThreadPool::Task task;
                {
                    std::lock_guard<decltype(m_tailMutex)> const guard(
                                m_tailMutex);
                    if (!m_threadPool) {
                        /* Deallocation of sliceTask will be handled by the
                           std::shared_ptr instance to this Inner object
                           instead. */
                        assert(!m_sliceTask);
                        m_sliceTask = std::move(sliceTask);
                        m_joinCond.notify_all();
                        return;
                    }
                    assert(m_head);
                    assert(m_head.get() != m_tail);
                    assert(m_head->m_value);
                    assert(m_head->m_next);
                    task = std::move(m_head);
                    m_head = std::move(task->m_next);
                }
                ThreadPool::TaskWrapper * const taskPtr = task.get();
                assert(taskPtr);
                assert(taskPtr->m_value);

                // Execute the retrieved task:
                taskPtr->m_value->operator()(std::move(task));
            }

            std::lock_guard<decltype(m_tailMutex)> tailGuard(m_tailMutex);
            if (m_threadPool && (m_head.get() != m_tail)) {
                m_threadPool->submit(std::move(sliceTask));
            } else {
                /* Deallocation of sliceTask will be handled by the
                   std::shared_ptr instance to this Inner object instead. */
                assert(!m_sliceTask);
                m_sliceTask = std::move(sliceTask);
                m_joinCond.notify_all();
            }
        }

    /* Fields: */

        std::shared_ptr<ThreadPool> m_threadPool;
        mutable TicketSpinLock m_tailMutex;
        std::condition_variable_any m_joinCond;
        ThreadPool::Task m_head{new ThreadPool::TaskWrapper(nullptr)};
        ThreadPool::TaskWrapper * m_tail{m_head.get()};
        ThreadPool::Task m_sliceTask;

    };

public: /* Methods: */

    Strand(std::shared_ptr<ThreadPool> threadPool)
        : m_internal(std::make_shared<Internal>(std::move(threadPool)))
    {
        std::weak_ptr<Internal> weakInternal(m_internal);
        m_internal->m_sliceTask =
                ThreadPool::createTask(
                    [weakInternal](ThreadPool::Task && sliceTask) noexcept {
                        if (auto const self = weakInternal.lock())
                            self->run(std::move(sliceTask));
                    });
    }

    ~Strand() noexcept { m_internal->stopAndJoin(); }

    void submit(ThreadPool::Task task) noexcept
    { m_internal->submit(std::move(task)); }

    std::shared_ptr<ThreadPool> stopAndJoin() noexcept
    { return m_internal->stopAndJoin(); }

    std::shared_ptr<ThreadPool> stopAndMaybeJoin() noexcept
    { return m_internal->stopAndMaybeJoin(); }

private: /* Fields: */

    std::shared_ptr<Internal> m_internal;

}; /* struct Strand */

} /* namespace sharemind {*/

#endif /* SHAREMIND_STRAND_H */
