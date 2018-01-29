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


namespace sharemind {

class TimeoutsThread {

public: /* Types: */

    using Clock =
        typename std::conditional<
            std::chrono::high_resolution_clock::is_steady,
            std::chrono::high_resolution_clock,
            std::chrono::steady_clock
        >::type;

private: /* Types: */


    struct Task
            : boost::intrusive::set_base_hook<
                boost::intrusive::link_mode<boost::intrusive::normal_link> >
    {

    /* Methods: */

        virtual ~Task() noexcept {}
        virtual void operator()() = 0; // Not explicitly noexcept for StopTask

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

    /* Fields: */

        Clock::time_point m_timePoint;
        std::shared_ptr<Task> m_selfPtr;

    };

    template <typename F>
    struct TaskImpl: Task {

    /* Methods: */

        template <typename ... Args>
        TaskImpl(Args && ... args) : m_f(std::forward<Args>(args)...) {}

        void operator()() final override { m_f(); }

    /* Fields: */

        F m_f;

    };

    struct StopException {};

    struct StopTask final: Task {

    /* Methods: */

        void operator()() final override { throw StopException(); }

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

        struct Disposer {
            void operator()(Task * const t) const noexcept
            { t->m_selfPtr.reset(); }
        };

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

        void moveExpiredTo(Tasks & targetContainer) noexcept {
            m_data.erase_and_dispose(
                    m_data.begin(),
                    m_data.upper_bound(
                        Clock::now(),
                        [](decltype(Clock::now()) const & t, Task const & a)
                                noexcept
                        { return t <= a.m_timePoint; }),
                    [&targetContainer](Task * const e) noexcept
                    { targetContainer.m_data.push_back(*e); });
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
    static std::shared_ptr<Task> createTask(F && f) {
        static_assert(
            noexcept(std::declval<typename std::decay<F>::type &>()()),
            "Callable for task must be noexcept!");
        return std::make_shared<TaskImpl<typename std::decay<F>::type> >(
            std::forward<F>(f));
    }

    void addTimeoutTask(Clock::duration const & duration,
                        std::shared_ptr<Task> task) noexcept
    { return addTask(Clock::now() + duration, std::move(task)); }

    template <typename TimePoint>
    void addTask(TimePoint && timePoint, std::shared_ptr<Task> task) noexcept {
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
                decltype(m_tasks) runTasks; // Container for expired tasks
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    // Wait for any tasks:
                    m_cond.wait(
                            lock,
                            [this]() noexcept { return !m_tasks.empty(); });
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
                    m_tasks.moveExpiredTo(runTasks);
                } // Release lock
                // Execute expired tasks in order:
                for (auto & runTask : runTasks)
                    runTask(); // May throw StopException
            }
        } catch (StopException const &) { return; }
    }

    void stop() noexcept {
        if (m_stopTask)
            addTask(Clock::time_point(), std::move(m_stopTask));
    }

private: /* Fields: */

    std::shared_ptr<Task> const m_stopTask{std::make_shared<StopTask>()};
    std::mutex m_mutex;
    std::condition_variable m_cond;
    Tasks m_tasks;

    std::thread m_thread{&TimeoutsThread::run, this};

};

} /* namespace sharemind */

#endif /* SHAREMIND_TIMEOUTSTHREAD_H */
