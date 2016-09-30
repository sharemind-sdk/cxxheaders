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

/** \todo Maybe this affects other setups as well? */
#include <sharemind/compiler-support/GccVersion.h>
#if defined(SHAREMIND_GCC_VERSION) && SHAREMIND_GCC_VERSION < 40800
#define SHAREMIND_THREADPOOL_USING_GCC47_SLOW_TICKETSPINLOCK_WORKAROUND
#endif

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
#ifndef SHAREMIND_THREADPOOL_USING_GCC47_SLOW_TICKETSPINLOCK_WORKAROUND
#include "TicketSpinLock.h"
#endif


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

    inline void joinFromThread() noexcept {
        std::thread::id const myId{std::this_thread::get_id()};
        std::unique_lock<std::mutex> const lock(m_threadsMutex,
                                                std::try_to_lock_t());
        if (lock.owns_lock()) {
            auto it = m_threads.begin();
            assert(it != m_threads.end());
            do {
                if (it->joinable()) {
                    if (it->get_id() == myId) {
                        while (++it != m_threads.end())
                            if (it->joinable())
                                it->join();
                        return;
                    }
                    it->join();
                }
            } while (++it != m_threads.end());
        }
    }

    inline void stopAndJoinFromThread() noexcept {
        if (notifyStop())
            return;
        joinFromThread();
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
    #if SHAREMIND_THREADPOOL_USING_GCC47_SLOW_TICKETSPINLOCK_WORKAROUND
    std::mutex m_tailMutex;
    std::condition_variable m_dataCond;
    #else
    TicketSpinLock m_tailMutex;
    std::condition_variable_any m_dataCond;
    #endif
    TaskWrapper * m_tail;
    Task m_head;
    bool m_stop = false;

    std::mutex m_threadsMutex;
    Pool m_threads;

    std::atomic_flag m_stopStarted = ATOMIC_FLAG_INIT;

}; /* class ThreadPool { */

} /* namespace sharemind {*/

#undef SHAREMIND_THREADPOOL_USING_GCC47_SLOW_TICKETSPINLOCK_WORKAROUND

#endif /* SHAREMIND_THREADPOOL_H */
