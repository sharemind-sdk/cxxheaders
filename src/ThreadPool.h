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

#ifndef SHAREMIND_THREADPOOL_H
#define SHAREMIND_THREADPOOL_H

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <sharemind/compiler-support/ClangVersion.h>
#include <thread>
#include <type_traits>
#include <utility>
#include "CallStack.h"
#include "StrongType.h"
#include "TicketSpinLock.h"


namespace sharemind {

class ThreadPool {

private: /* Forward declarations: */

    struct TaskWrapper;

public: /* Types: */

    using Task = std::unique_ptr<TaskWrapper>;

    class ReusableTask: public Task {

    public: /* Methods: */

        template <typename F>
        ReusableTask(F && f)
            : Task(createTask([this, f](Task && task) noexcept {
                                  this->Task::operator=(std::move(task));
                                  f();
                              }))
        {}

    };

    /**
      \brief Allows queueing of serial jobs on any thread pool.

      This class provides means to utilize a thread pool to execute jobs which
      are not allowed to run in parallel. Even if the thread pool has N > 1
      threads, no two jobs queued by this class will be executed in parallel.
    */
    class OneThreadSharedSlice {

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

            void submit(Task && task) noexcept {
                assert(task);
                assert(task->m_value);
                assert(!task->m_next);
                TaskWrapper * const newTail = task.get();
                std::lock_guard<decltype(m_tailMutex)> const guard(m_tailMutex);
                TaskWrapper * const oldTail = m_tail;
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

            void run(Task && sliceTask) noexcept {
                CallStack<CallStackRecursionIndicator>::Context context(this);
                {
                    // Retrieve first task (or return if stopping):
                    Task task;
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
                    TaskWrapper * const taskPtr = task.get();
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
            Task m_head{new TaskWrapper(nullptr)};
            TaskWrapper * m_tail{m_head.get()};
            Task m_sliceTask;

        };

    public: /* Methods: */

        OneThreadSharedSlice(std::shared_ptr<ThreadPool> threadPool)
            : m_internal(std::make_shared<Internal>(std::move(threadPool)))
        {
            std::weak_ptr<Internal> weakInternal(m_internal);
            m_internal->m_sliceTask =
                    ThreadPool::createTask(
                        [weakInternal](Task && sliceTask) noexcept {
                            if (auto const self = weakInternal.lock())
                                self->run(std::move(sliceTask));
                        });
        }

        ~OneThreadSharedSlice() noexcept { m_internal->stopAndJoin(); }

        void submit(Task task) noexcept
        { m_internal->submit(std::move(task)); }

        std::shared_ptr<ThreadPool> stopAndJoin() noexcept
        { return m_internal->stopAndJoin(); }

        std::shared_ptr<ThreadPool> stopAndMaybeJoin() noexcept
        { return m_internal->stopAndMaybeJoin(); }

    private: /* Fields: */

        std::shared_ptr<Internal> m_internal;

    }; /* struct SharedSlice */

private: /* Types: */

    struct TaskBase {
        TaskBase() {}
        virtual ~TaskBase() noexcept {}
        TaskBase(TaskBase &&) = delete;
        TaskBase(TaskBase const &) = delete;
        TaskBase & operator=(TaskBase &&) = delete;
        TaskBase & operator=(TaskBase const &) = delete;

        virtual void operator()(Task &&) noexcept = 0;
    };

    struct TaskWrapper final {
        TaskWrapper(std::unique_ptr<TaskBase> && value) noexcept
            : m_value(std::move(value))
        {}

        std::unique_ptr<TaskBase> m_value;
        Task m_next;
    };

public: /* Methods: */

    ThreadPool(ThreadPool &&) = delete;
    ThreadPool(ThreadPool const &) = delete;
    ThreadPool & operator=(ThreadPool &&) = delete;
    ThreadPool & operator=(ThreadPool const &) = delete;

    virtual ~ThreadPool() noexcept {}

    template <typename F>
    static Task createTask(F && f) {
        struct CustomTask: TaskBase {
            CustomTask(F && f) : m_f(std::forward<F>(f)) {}
            void operator()(Task && task) noexcept final override
            { m_f(std::move(task)); }
            typename std::decay<F>::type m_f;
        };
        return createTask_(new CustomTask(std::forward<F>(f)));
    }

    template <typename F>
    static Task createSimpleTask(F && f) {
        struct CustomSimpleTask: TaskBase {
            CustomSimpleTask(F && f) : m_f(std::forward<F>(f)) {}
            void operator()(Task &&) noexcept final override { m_f(); }
            typename std::decay<F>::type m_f;
        };
        return createTask_(new CustomSimpleTask(std::forward<F>(f)));
    }

    void submit(Task task) noexcept {
        assert(task);
        assert(task->m_value);
        assert(!task->m_next);
        TaskWrapper * const newTail = task.get();
        std::lock_guard<decltype(m_tailMutex)> const tailGuard(m_tailMutex);
        TaskWrapper * const oldTail = m_tail;
        oldTail->m_value = std::move(task->m_value);
        oldTail->m_next = std::move(task);
        m_tail = newTail;
        m_dataCond.notify_one();
    }

    void notifyStop() noexcept {
        std::lock_guard<decltype(m_tailMutex)> const tailGuard(m_tailMutex);
        m_stop = true;
        m_dataCond.notify_all();
    }

protected: /* Methods: */

    ThreadPool() : ThreadPool(new TaskWrapper(nullptr)) {}

    void workerThread() noexcept {
        while (Task task = waitAndPop()) {
            // this->m_value(std::move(*this)); // would segfault.
            TaskWrapper * const taskPtr = task.get();
            assert(taskPtr);
            assert(taskPtr->m_value);
            taskPtr->m_value->operator()(std::move(task));
        }
    }

private: /* Methods: */

    ThreadPool(TaskWrapper * const emptyTaskWrapper)
        : m_tail(emptyTaskWrapper)
        , m_head(emptyTaskWrapper)
    {}

    Task waitAndPop() noexcept {
        std::lock_guard<std::mutex> const headGuard(m_headMutex);
        assert(m_head);
        {
            std::unique_lock<decltype(m_tailMutex)> tailLock(m_tailMutex);
            for (;;) {
                if (m_stop)
                    return Task();
                if (m_head.get() != m_tail)
                    break;
                #if defined(SHAREMIND_CLANG_VERSION) \
                    && (SHAREMIND_CLANG_VERSION < 30700)
                #warning Clang 3.6 (and possibly older versions) are known to \
                         sometimes hang here for unknown reasons!
                #endif
                m_dataCond.wait(tailLock);
            }
        }
        assert(m_head->m_value);
        Task oldHead(std::move(m_head));
        m_head = std::move(oldHead->m_next);
        return oldHead;
    }

    template <typename TaskSubclass>
    static Task createTask_(TaskSubclass * const task) {
        assert(task);
        return Task(new TaskWrapper(std::unique_ptr<TaskBase>(task)));
    }

private: /* Fields: */

    std::mutex m_headMutex;
    TicketSpinLock m_tailMutex;
    std::condition_variable_any m_dataCond;
    TaskWrapper * m_tail;
    Task m_head;
    bool m_stop = false;

}; /* class ThreadPool { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_THREADPOOL_H */
