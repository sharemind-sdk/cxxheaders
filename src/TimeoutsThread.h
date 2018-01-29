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

#ifndef SHAREMIND_TIMEOUTSTHREAD_H
#define SHAREMIND_TIMEOUTSTHREAD_H

#include <boost/intrusive/set.hpp>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include "MakeUnique.h"


namespace sharemind {

class TimeoutsThread {

private: /* Types: */

    class Tasks;

public: /* Types: */

    using Clock =
        typename std::conditional<
            std::chrono::high_resolution_clock::is_steady,
            std::chrono::high_resolution_clock,
            std::chrono::steady_clock
        >::type;

    class Task
            : public boost::intrusive::set_base_hook<
                boost::intrusive::link_mode<boost::intrusive::normal_link> >
    {

        friend class TimeoutsThread;
        friend class Tasks;

    public: /* Methods: */

        virtual ~Task() noexcept {}

        /// \note Not explicitly noexcept for StopTask
        virtual void operator()(std::unique_ptr<Task> &&) = 0;

        bool operator==(Task const & rhs) const noexcept
        { return m_timePoint == rhs.m_timePoint; }

        bool operator!=(Task const & rhs) const noexcept
        { return m_timePoint != rhs.m_timePoint; }

        bool operator<(Task const & rhs) const noexcept
        { return m_timePoint < rhs.m_timePoint; }

        bool operator<=(Task const & rhs) const noexcept
        { return m_timePoint <= rhs.m_timePoint; }

        bool operator>(Task const & rhs) const noexcept
        { return m_timePoint > rhs.m_timePoint; }

        bool operator>=(Task const & rhs) const noexcept
        { return m_timePoint >= rhs.m_timePoint; }

    private: /* Fields: */

        Clock::time_point m_timePoint;
        std::unique_ptr<Task> m_selfPtr;

    };

private: /* Types: */

    struct StopException {};

    struct StopTask final: Task {

    /* Methods: */

        void operator()(std::unique_ptr<Task> &&) final override
        { throw StopException(); }

    };

    class Tasks {

    private: /* Types: */

        using Inner = boost::intrusive::multiset<
            Task,
            boost::intrusive::constant_time_size<false>
        >;

    public: /* Types: */

        using iterator = Inner::iterator;
        using const_iterator = Inner::const_iterator;
        using reference = Inner::reference;

    public: /* Methods: */

        ~Tasks() noexcept {
            m_data.clear_and_dispose(
                [](Task * const t) noexcept { t->m_selfPtr.reset(); });
        }

        iterator begin() noexcept { return m_data.begin(); }
        const_iterator begin() const noexcept { return m_data.begin(); }

        iterator end() noexcept { return m_data.end(); }
        const_iterator end() const noexcept { return m_data.end(); }

        iterator insert(reference t) noexcept { return m_data.insert(t); }

        bool empty() const noexcept { return m_data.empty(); }

        std::unique_ptr<Task> takeFirstTask() noexcept {
            std::unique_ptr<Task> r;
            m_data.erase_and_dispose(
                        m_data.begin(),
                        [&r](Task * const task) noexcept {
                            assert(task->m_selfPtr.get() == task);
                            r = std::move(task->m_selfPtr);
                        });
            return r;
        }

    private: /* Fields: */

        boost::intrusive::multiset<
            Task,
            boost::intrusive::constant_time_size<false>
        > m_data;

    };

public: /* Methods: */

    ~TimeoutsThread() noexcept {
        stop();
        m_thread.join();
    }

    template <typename F>
    static std::unique_ptr<Task> createOneShotTask(F && f) {
        static_assert(
            noexcept(std::declval<typename std::decay<F>::type &>()()),
            "Callable for task must be noexcept!");

        struct TaskImpl: Task {

        /* Methods: */

            TaskImpl(F && f) : m_f(std::forward<F>(f)) {}

            void operator()(std::unique_ptr<Task> &&) final override { m_f(); }

        /* Fields: */

            typename std::decay<F>::type m_f;

        };

        return makeUnique<TaskImpl>(std::forward<F>(f));
    }

    template <typename F>
    static std::unique_ptr<Task> createReusableTask(F && f) {
        static_assert(
            noexcept(std::declval<typename std::decay<F>::type &>()(
                         std::declval<std::unique_ptr<Task> &&>())),
            "Callable for task must be noexcept!");

        struct TaskImpl: Task {

        /* Methods: */

            TaskImpl(F && f) : m_f(std::forward<F>(f)) {}

            void operator()(std::unique_ptr<Task> && task) final override
            { m_f(std::move(task)); }

        /* Fields: */

            typename std::decay<F>::type m_f;

        };

        return makeUnique<TaskImpl>(std::forward<F>(f));
    }

    void addTimeoutTask(Clock::duration const & duration,
                        std::unique_ptr<Task> task) noexcept
    { return addTask(Clock::now() + duration, std::move(task)); }

    template <typename TimePoint>
    void addTask(TimePoint && timePoint, std::unique_ptr<Task> task) noexcept {
        assert(task);
        auto & t = *task;
        assert(!t.m_selfPtr); // Already inserted
        t.m_selfPtr = std::move(task);
        t.m_timePoint = std::forward<TimePoint>(timePoint);
        std::lock_guard<std::mutex> const guard(m_mutex);
        m_tasks.insert(t);
        m_cond.notify_one();
    }

    void run() noexcept {
        try {
            for (;;) {
                std::unique_ptr<Task> runTask;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    // Wait for any tasks:
                    while (m_tasks.empty())
                        m_cond.wait(lock);
                    for (;;) {
                        // Expiry of earliest task:
                        auto const nextTimePoint(m_tasks.begin()->m_timePoint);

                        // Earliest task already expiring?
                        if (nextTimePoint <= Clock::now())
                            break;

                        // Wait for expiry or new task
                        if (m_cond.wait_until(lock, nextTimePoint)
                            == std::cv_status::timeout)
                            break;
                    }
                    // Move any expired/expiring tasks to runTasks.
                    runTask = m_tasks.takeFirstTask();
                } // Release lock

                // Execute expired task:
                assert(runTask);
                auto & runTaskRef = *runTask.get();
                runTaskRef(std::move(runTask));
            }
        } catch (StopException const &) { return; }
    }

    void stop() noexcept {
        if (m_stopTask)
            addTask(Clock::time_point(), std::move(m_stopTask));
    }

private: /* Fields: */

    std::unique_ptr<Task> m_stopTask{makeUnique<StopTask>()};
    std::mutex m_mutex;
    std::condition_variable m_cond;
    Tasks m_tasks;

    std::thread m_thread{&TimeoutsThread::run, this};

};

} /* namespace sharemind */

#endif /* SHAREMIND_TIMEOUTSTHREAD_H */
