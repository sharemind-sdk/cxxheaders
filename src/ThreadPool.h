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

#include <cassert>
#include <condition_variable>
#include <functional>
#include <memory>
#include <sharemind/QueueingMutex.h>
#include <thread>
#include <vector>
#include <type_traits>


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

        // May only throw StopThreadException:
        virtual void operator()(Task &&) = 0;
    };

    struct TaskWrapper final {
        inline TaskWrapper(std::unique_ptr<TaskBase> && value) noexcept
            : m_value{std::move(value)}
        {}

        std::unique_ptr<TaskBase> m_value;
        Task m_next;
    };

    class StopThreadException {};

    using Mutex = sharemind::QueueingMutex;
    using Lock = sharemind::QueueingMutex::Lock;
    using Guard = sharemind::QueueingMutex::Guard;

public: /* Methods: */

    ThreadPool(ThreadPool const &) = delete;
    ThreadPool & operator=(ThreadPool const &) = delete;

    inline ThreadPool()
        : ThreadPool{[]{
                unsigned const n = std::thread::hardware_concurrency();
                return (n == 0u) ? 3u : n;
            }()}
    {}

    inline ThreadPool(size_t const numThreads)
        : ThreadPool{(assert(numThreads > 0u), numThreads),
                     new TaskWrapper{nullptr}}
    {}

    virtual inline ~ThreadPool() noexcept { stop(); }

    template <typename F>
    static inline Task createTask(F f) {
        struct CustomTask: TaskBase {
            inline CustomTask(F f) : m_f{std::move(f)} {}
            inline void operator()(Task && task) final override
            { m_f(std::move(task)); }
            F m_f;
        };
        return createTask(new CustomTask{std::move(f)});
    }

    template <typename F>
    static inline Task createSimpleTask(F f) {
        struct CustomSimpleTask: TaskBase {
            inline CustomSimpleTask(F f) : m_f{std::move(f)} {}
            inline void operator()(Task &&) final override { m_f(); }
            F m_f;
        };
        return createTask(new CustomSimpleTask{std::move(f)});
    }

    inline void submit(Task task) noexcept {
        assert(task);
        assert(task->m_value);
        TaskWrapper * const newTail = task.get();
        Guard const tailGuard{m_tailMutex};
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

    inline ThreadPool(size_t const numThreads,
                      TaskWrapper * const emptyTaskWrapper)
        : m_tail{emptyTaskWrapper}
        , m_head{emptyTaskWrapper}
    {
        m_stopTasks.reserve(numThreads);
        m_threads.reserve(numThreads);
        for (unsigned i = 0u; i < numThreads; i++)
            m_stopTasks.emplace_back(
                        createTask([i](Task)
                                   { throw StopThreadException{}; }));
        try {
            for (unsigned i = 0u; i < numThreads; i++)
                m_threads.emplace_back(&ThreadPool::workerThread, this);
        } catch (...) {
            stop();
            throw;
        }
    }

    inline Task waitAndPop() noexcept {
        Guard const headGuard{m_headMutex};
        assert(m_head);
        {
            Lock tailLock{m_tailMutex};
            while (m_head.get() == m_tail)
                #ifdef __clang__
                #warning Clang 3.6 (and possibly other versions) are known to \
                         sometimes hang here for unknown reasons!
                #endif
                m_dataCond.wait(tailLock);
        }
        assert(m_head->m_value);
        Task oldHead{std::move(m_head)};
        m_head = std::move(oldHead->m_next);
        return oldHead;
    }

    inline void stop() noexcept {
        assert(m_stopTasks.size() >= m_threads.size());
        size_t const maxStopJobs = m_threads.size();
        for (size_t i = 0u; i < maxStopJobs; i++)
            submit(std::move(m_stopTasks[i]));
        for (std::thread & thread : m_threads)
            if (thread.joinable())
                thread.join();
    }

    inline void workerThread() noexcept {
        for (;;) {
            Task task{waitAndPop()};
            // this->m_value(std::move(*this)); // would segfault.
            TaskWrapper * const taskPtr = task.get();
            assert(taskPtr);
            assert(taskPtr->m_value);
            try {
                taskPtr->m_value->operator()(std::move(task));
            } catch (StopThreadException const &) { return; }
        }
    }

private: /* Fields: */

    Mutex m_headMutex;
    Mutex m_tailMutex;
    std::condition_variable_any m_dataCond;
    TaskWrapper * m_tail;
    Task m_head;

    std::vector<std::thread> m_threads;
    std::vector<Task> m_stopTasks;

}; /* class ThreadPool { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_THREADPOOL_H */
