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

#ifndef SHAREMIND_THREADPOOL_H
#define SHAREMIND_THREADPOOL_H

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <sharemind/compiler-support/ClangVersion.h>
#include <thread>
#include <vector>
#include <type_traits>
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
        inline ReusableTask(F && f)
            : Task{createTask([this, f](Task && task) noexcept {
                                  this->Task::operator=(std::move(task));
                                  f();
                              })}
        {}

    };

    /**
      \brief Allows queueing of serial jobs on any thread pool.

      This class provides means to utilize a thread pool to execute jobs which
      are not allowed to run in parallel. Even if the thread pool has N > 1
      threads, no two jobs queued by this class will be executed in parallel.
    */
    class OneThreadSharedSlice {

    public: /* Methods: */

        inline OneThreadSharedSlice(std::shared_ptr<ThreadPool> threadPool)
            : m_threadPool(std::move(threadPool))
        {}

        inline ~OneThreadSharedSlice() noexcept { notifyStop(); }

        inline void init(std::shared_ptr<OneThreadSharedSlice> sharedSelf) {
            assert(sharedSelf.get() == this);
            std::weak_ptr<OneThreadSharedSlice> weakSelf(sharedSelf);
            #ifndef NDEBUG
            std::lock_guard<decltype(m_tailMutex)> const guard(m_tailMutex);
            #endif
            assert(m_tail == m_head.get());
            m_sliceTask =
                    ThreadPool::createTask(
                        [weakSelf](Task && sliceTask) noexcept {
                            if (auto const self = weakSelf.lock())
                              self->run(std::move(sliceTask));
                        });
        }

        inline void submit(Task task) noexcept {
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

        inline std::shared_ptr<ThreadPool> notifyStop() noexcept {
            std::shared_ptr<ThreadPool> gcThreadPool;
            std::lock_guard<decltype(m_tailMutex)> tailGuard(m_tailMutex);
            gcThreadPool = std::move(m_threadPool);
            assert(!m_threadPool);
            return gcThreadPool;
        }

        inline bool runningFromThisThread() const noexcept
        { return runningFromThread(std::this_thread::get_id()); }

        inline bool runningFromThread(std::thread::id const & id) const noexcept
        {
            std::lock_guard<decltype(m_tailMutex)> const guard(m_tailMutex);
            return m_running && (m_lastRunningThreadId == id);
        }

    private: /* Methods: */

        inline void run(Task && sliceTask) noexcept {
            {
                // Retrieve first task (or return if stopping):
                Task task;
                auto thisThreadId(std::this_thread::get_id());
                {
                    std::lock_guard<decltype(m_tailMutex)> const guard(
                                m_tailMutex);
                    if (!m_threadPool) {
                        /* Deallocation of sliceTask will be handled by the
                           std::shared_ptr instance to this Inner object
                           instead. */
                        assert(!m_sliceTask);
                        m_sliceTask = std::move(sliceTask);
                        return;
                    }
                    assert(m_head);
                    assert(m_head.get() != m_tail);
                    assert(m_head->m_value);
                    assert(m_head->m_next);
                    task = std::move(m_head);
                    m_head = std::move(task->m_next);
                    m_running = true;
                    m_lastRunningThreadId = std::move(thisThreadId);
                }
                TaskWrapper * const taskPtr = task.get();
                assert(taskPtr);
                assert(taskPtr->m_value);

                // Execute the retrieved task:
                taskPtr->m_value->operator()(std::move(task));
            }

            std::lock_guard<decltype(m_tailMutex)> tailGuard(m_tailMutex);
            m_running = false;
            if (m_threadPool && (m_head.get() != m_tail)) {
                m_threadPool->submit(std::move(sliceTask));
            } else {
                /* Deallocation of sliceTask will be handled by the
                   std::shared_ptr instance to this Inner object instead. */
                assert(!m_sliceTask);
                m_sliceTask = std::move(sliceTask);
            }
        }

    private: /* Fields: */

        std::shared_ptr<ThreadPool> m_threadPool;
        mutable TicketSpinLock m_tailMutex;
        Task m_head{new TaskWrapper(nullptr)};
        TaskWrapper * m_tail{m_head.get()};
        Task m_sliceTask;
        bool m_running = false;
        std::thread::id m_lastRunningThreadId;

    }; /* struct SharedSlice */

private: /* Types: */

    struct TaskBase {
        inline TaskBase() {}
        virtual inline ~TaskBase() noexcept {}
        TaskBase(TaskBase &&) = delete;
        TaskBase(TaskBase const &) = delete;
        TaskBase & operator=(TaskBase &&) = delete;
        TaskBase & operator=(TaskBase const &) = delete;

        virtual void operator()(Task &&) noexcept = 0;
    };

    struct TaskWrapper final {
        inline TaskWrapper(std::unique_ptr<TaskBase> && value) noexcept
            : m_value{std::move(value)}
        {}

        std::unique_ptr<TaskBase> m_value;
        Task m_next;
    }
    ;
    using Pool = std::vector<std::thread>;

public: /* Methods: */

    ThreadPool(ThreadPool const &) = delete;
    ThreadPool & operator=(ThreadPool const &) = delete;

    inline ThreadPool()
        : ThreadPool{[]{
                unsigned const n = std::thread::hardware_concurrency();
                return (n == 0u) ? 3u : n;
            }()}
    {}

    inline ThreadPool(Pool::size_type const numThreads)
        : ThreadPool{(assert(numThreads > 0u), numThreads),
                     new TaskWrapper{nullptr}}
    {}

    virtual inline ~ThreadPool() noexcept {
        assert((std::find_if(
                    m_threads.begin(),
                    m_threads.end(),
                    [](std::thread const & t)
                    { return t.get_id() == std::this_thread::get_id(); })
                == m_threads.end()) && "Can't destroy pool from pool thread!");
        stopAndJoin();
    }

    /// \returns whether notifyStop() had already been called.
    bool notifyStop() noexcept {
        if (m_stopStarted.test_and_set())
            return true;
        std::lock_guard<decltype(m_tailMutex)> const tailGuard(m_tailMutex);
        m_stop = true;
        m_dataCond.notify_all();
        return false;
    }

    inline void join() noexcept {
        #ifndef NDEBUG
        std::thread::id const myId{std::this_thread::get_id()};
        #endif
        std::lock_guard<std::mutex> const guard(m_threadsMutex);
        for (std::thread & thread : m_threads)
            if ((assert(thread.get_id() != myId), thread.joinable()))
                thread.join();
    }

    inline void stopAndJoin() noexcept {
        notifyStop();
        join();
    }

    template <typename F>
    static inline Task createTask(F f) {
        struct CustomTask: TaskBase {
            inline CustomTask(F f) : m_f{std::move(f)} {}
            inline void operator()(Task && task) noexcept final override
            { m_f(std::move(task)); }
            F m_f;
        };
        return createTask(new CustomTask{std::move(f)});
    }

    template <typename F>
    static inline Task createSimpleTask(F f) {
        struct CustomSimpleTask: TaskBase {
            inline CustomSimpleTask(F f) : m_f{std::move(f)} {}
            inline void operator()(Task &&) noexcept final override { m_f(); }
            F m_f;
        };
        return createTask(new CustomSimpleTask{std::move(f)});
    }

    inline void submit(Task task) noexcept {
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

private: /* Methods: */

    static inline Task createTask(std::unique_ptr<TaskBase> task) {
        assert(task);
        return Task{new TaskWrapper{std::move(task)}};
    }

    template <typename TaskSubclass>
    static inline Task createTask(TaskSubclass * const task) {
        assert(task);
        return Task{new TaskWrapper{std::unique_ptr<TaskBase>{task}}};
    }

    inline ThreadPool(Pool::size_type const numThreads,
                      TaskWrapper * const emptyTaskWrapper)
        : m_tail{emptyTaskWrapper}
        , m_head{emptyTaskWrapper}
    {
        m_threads.reserve(numThreads);
        try {
            for (unsigned i = 0u; i < numThreads; i++)
                m_threads.emplace_back(&ThreadPool::workerThread, this);
        } catch (...) {
            stopAndJoin();
            throw;
        }
    }

    inline Task waitAndPop() noexcept {
        std::lock_guard<std::mutex> const headGuard(m_headMutex);
        assert(m_head);
        {
            std::unique_lock<decltype(m_tailMutex)> tailLock(m_tailMutex);
            for (;;) {
                if (m_stop)
                    return Task{};
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
        Task oldHead{std::move(m_head)};
        m_head = std::move(oldHead->m_next);
        return oldHead;
    }

    inline void workerThread() noexcept {
        for (;;) {
            Task task{waitAndPop()};
            if (!task)
                return;
            // this->m_value(std::move(*this)); // would segfault.
            TaskWrapper * const taskPtr = task.get();
            assert(taskPtr);
            assert(taskPtr->m_value);
            taskPtr->m_value->operator()(std::move(task));
        }
    }

private: /* Fields: */

    std::mutex m_headMutex;
    TicketSpinLock m_tailMutex;
    std::condition_variable_any m_dataCond;
    TaskWrapper * m_tail;
    Task m_head;
    bool m_stop = false;

    std::mutex m_threadsMutex;
    Pool m_threads;

    std::atomic_flag m_stopStarted = ATOMIC_FLAG_INIT;

}; /* class ThreadPool { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_THREADPOOL_H */
