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

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <sharemind/compiler-support/ClangVersion.h>
#include <type_traits>
#include <utility>
#include "TicketSpinLock.h"


namespace sharemind {

class Strand;

class ThreadPool {

    friend class Strand;

private: /* Forward declarations: */

    struct TaskWrapper;

public: /* Types: */

    enum Status { Ok, Timeout, StopNotified };

    using Task = std::unique_ptr<TaskWrapper>;

    class ReusableTask: public Task {

    public: /* Methods: */

        template <typename F>
        ReusableTask(F && f)
            : Task(createTask([this, f](Task && task) {
                                  this->Task::operator=(std::move(task));
                                  f();
                              }))
        {}

    };

private: /* Types: */

    struct TaskBase {
        TaskBase() {}
        virtual ~TaskBase() noexcept {}
        TaskBase(TaskBase &&) = delete;
        TaskBase(TaskBase const &) = delete;
        TaskBase & operator=(TaskBase &&) = delete;
        TaskBase & operator=(TaskBase const &) = delete;

        virtual void operator()(Task &&) = 0;
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
            void operator()(Task && task) final override
            { m_f(std::move(task)); }
            typename std::decay<F>::type m_f;
        };
        return createTask_(new CustomTask(std::forward<F>(f)));
    }

    template <typename F>
    static Task createSimpleTask(F && f) {
        struct CustomSimpleTask: TaskBase {
            CustomSimpleTask(F && f) : m_f(std::forward<F>(f)) {}
            void operator()(Task &&) final override { m_f(); }
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

    bool stopNotified() const noexcept {
        std::lock_guard<decltype(m_tailMutex)> const tailGuard(m_tailMutex);
        return m_stop;
    }

protected: /* Methods: */

    ThreadPool() : ThreadPool(new TaskWrapper(nullptr)) {}

    void workerThread() {
        while (Task task = waitAndPop()) {
            // this->m_value(std::move(*this)); // would segfault.
            TaskWrapper * const taskPtr = task.get();
            assert(taskPtr);
            assert(taskPtr->m_value);
            taskPtr->m_value->operator()(std::move(task));
        }
    }

    template <typename Clock, typename Duration>
    Status workerThreadUntil(
            std::chrono::time_point<Clock, Duration> const & timepoint)
    {
        for (;;) {
            auto r(waitAndPop(timepoint));
            if (r.first) {
                // this->m_value(std::move(*this)); // would segfault.
                TaskWrapper * const taskPtr = r.first.get();
                assert(taskPtr);
                assert(taskPtr->m_value);
                taskPtr->m_value->operator()(std::move(r.first));
            } else {
                return r.second;
            }
        }
    }

    bool oneTaskWorkerThread() {
        if (Task task = waitAndPop()) {
            // this->m_value(std::move(*this)); // would segfault.
            TaskWrapper * const taskPtr = task.get();
            assert(taskPtr);
            assert(taskPtr->m_value);
            taskPtr->m_value->operator()(std::move(task));
            return true;
        }
        return false;
    }

    template <typename Clock, typename Duration>
    Status oneTaskWorkerThreadUntil(
            std::chrono::time_point<Clock, Duration> const & timepoint)
    {
        auto r(waitAndPop(timepoint));
        if (r.first) {
            // this->m_value(std::move(*this)); // would segfault.
            TaskWrapper * const taskPtr = r.first.get();
            assert(taskPtr);
            assert(taskPtr->m_value);
            taskPtr->m_value->operator()(std::move(r.first));
            return Ok;
        } else {
            return r.second;
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

    template <typename Clock, typename Duration>
    std::pair<Task, Status> waitAndPop(
            std::chrono::time_point<Clock, Duration> const & timepoint) noexcept
    {
        std::lock_guard<std::mutex> const headGuard(m_headMutex);
        assert(m_head);
        {
            std::unique_lock<decltype(m_tailMutex)> tailLock(m_tailMutex);
            for (;;) {
                if (m_stop)
                    return {Task(), StopNotified};
                if (m_head.get() != m_tail)
                    break;
                #if defined(SHAREMIND_CLANG_VERSION) \
                    && (SHAREMIND_CLANG_VERSION < 30700)
                #warning Clang 3.6 (and possibly older versions) are known to \
                         sometimes hang here for unknown reasons!
                #endif
                if (m_dataCond.wait_until(tailLock, timepoint)
                    == std::cv_status::timeout)
                    return {Task(), Timeout};
            }
        }
        assert(m_head->m_value);
        std::pair<Task, Status> r(std::move(m_head), Ok);
        m_head = std::move(r.first->m_next);
        return r;
    }

    template <typename TaskSubclass>
    static Task createTask_(TaskSubclass * const task) {
        assert(task);
        return Task(new TaskWrapper(std::unique_ptr<TaskBase>(task)));
    }

private: /* Fields: */

    std::mutex m_headMutex;
    mutable TicketSpinLock m_tailMutex;
    std::condition_variable_any m_dataCond;
    TaskWrapper * m_tail;
    Task m_head;
    bool m_stop = false;

}; /* class ThreadPool { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_THREADPOOL_H */
