/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_EVENTLOOP_H
#define SHAREMIND_EVENTLOOP_H

#include <cassert>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include "Exception.h"
#include "Posix.h"
#include "ScopeExit.h"
#include "Spinwait.h"

#if defined(__linux__)
#include <sys/epoll.h>
#else
#error Detected an operating system which is currently not supported!
#endif


namespace sharemind {

class EventLoop {

public: /* Constants: */

    static constexpr std::size_t const DEFAULT_MAX_EVENTS = 128u;
    static constexpr int const DEFAULT_EPOLL_TIMEOUT_MS = 50;

    #if defined(__linux__)
    using EventSet = std::uint32_t;
    static constexpr EventSet const ERROR_EVENTS = EPOLLERR;

    static constexpr EventSet const INPUT_DATA_EVENTS = EPOLLPRI | EPOLLIN;
    static constexpr EventSet const INPUT_HUP_EVENTS = EPOLLHUP | EPOLLRDHUP;
    static constexpr EventSet const INPUT_EVENTS =
            INPUT_DATA_EVENTS | INPUT_HUP_EVENTS;
    static constexpr EventSet const ALL_INPUT_EVENTS =
            INPUT_EVENTS | ERROR_EVENTS;

    static constexpr EventSet const OUTPUT_DATA_EVENTS = EPOLLOUT;
    static constexpr EventSet const OUTPUT_HUP_EVENTS = EPOLLHUP;
    static constexpr EventSet const OUTPUT_EVENTS =
            OUTPUT_DATA_EVENTS | OUTPUT_HUP_EVENTS;
    static constexpr EventSet const ALL_OUTPUT_EVENTS =
            OUTPUT_EVENTS | ERROR_EVENTS;

    static constexpr EventSet const COMMON_HUP_EVENTS =
            INPUT_HUP_EVENTS & OUTPUT_HUP_EVENTS;
    static constexpr EventSet const ALL_HUP_EVENTS =
            INPUT_HUP_EVENTS | OUTPUT_HUP_EVENTS;
    static constexpr EventSet const ALL_EVENTS =
            ALL_INPUT_EVENTS | ALL_OUTPUT_EVENTS;
    #endif

public: /* Types: */

    SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);

    #if defined(__linux__) || defined(__NetBSD__) \
        || defined(__OpenBSD__) || defined(__FreeBSD__)
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                         PipeCreateException,
                                        "pipe2() failed!");
    #else
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                         PipeCreateException,
                                        "pipe() failed!");
    #endif
    #if defined(__linux__)
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                         EpollCreateException,
                                         "epoll_create1() failed!");
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                         EpollCtlException,
                                         "epoll_ctl() failed!");
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                         EpollWaitException,
                                         "epoll_wait() failed!");
    #else
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                         FcntlException,
                                         "fcntl() failed!");
    #endif

    struct EventHandler {
        inline virtual ~EventHandler() noexcept {}
        virtual void handleEvents(EventSet events) noexcept = 0;
    };

    class ScopedThread {

    public: /* Methods: */

        inline ScopedThread(std::shared_ptr<EventLoop> eventLoop)
            : ScopedThread(std::move(eventLoop), []() noexcept {})
        {}

        template <typename ExceptionHandler>
        inline ScopedThread(std::shared_ptr<EventLoop> eventLoop,
                            ExceptionHandler && exceptionHandler)
            : m_eventLoop(eventLoop)
            , m_thread{[eventLoop,exceptionHandler]() noexcept {
                          try {
                              eventLoop->run();
                          } catch (...) {
                              static_assert(noexcept(exceptionHandler()), "");
                              exceptionHandler();
                          }
                       }}
        {}

        inline ~ScopedThread() noexcept {
            if (m_thread.joinable()) {
                if (auto const eventLoop = m_eventLoop.lock())
                    eventLoop->stop();
                m_thread.join();
            }
        }

    private: /* Fields: */

        std::weak_ptr<EventLoop> m_eventLoop;
        std::thread m_thread;

    };

private: /* Types: */

    struct Pipe {

    /* Methods: */

        inline Pipe() {
            int fds[2u];
            syscallLoop<PipeCreateException>(
                    #if defined(__linux__) || defined(__NetBSD__) \
                        || defined(__OpenBSD__) || defined(__FreeBSD__)
                        ::pipe2
                    #else
                #warning Cannot create pipe with CLOEXEC flags on this platform!
                        ::pipe
                    #endif
                        , [](int const r) noexcept { return r == 0; }
                        , fds
                    #if defined(__linux__) || defined(__NetBSD__) \
                        || defined(__OpenBSD__) || defined(__FreeBSD__)
                        , O_CLOEXEC | O_NONBLOCK
                    #if defined(__NetBSD__)
                          | O_NOSIGPIPE
                    #endif
                    #endif
                        );
            try {
                #if !defined(__linux__) && !defined(__NetBSD__) \
                    && !defined(__OpenBSD__) && !defined(__FreeBSD__)
                fcntlAddFd<FcntlException>(fd[0u], FD_CLOEXEC);
                fcntlAddFd<FcntlException>(fd[1u], FD_CLOEXEC);
                fcntlAddFl<FcntlException>(fd[0u], O_NONBLOCK);
                fcntlAddFl<FcntlException>(fd[1u], O_NONBLOCK);
                #endif
            } catch (...) {
                ::close(fds[0u]);
                ::close(fds[1u]);
                throw;
            }
            readEnd = fds[0u];
            writeEnd = fds[1u];
        }

        inline ~Pipe() noexcept {
            ::close(readEnd);
            if (readEnd != writeEnd)
                ::close(writeEnd);
        }

        inline void writeEndClose() noexcept {
            if (writeEnd == readEnd)
                return;
            ::close(writeEnd);
            writeEnd = readEnd;
        }

    /* Fields: */

        int readEnd;
        int writeEnd;

    };

private: /* Types: */

    #if defined(__linux__)
    struct Epoll {

    /* Methods: */

        inline Epoll()
            : fd(::epoll_create1(EPOLL_CLOEXEC))
        {
            using sharemind::ErrnoException;
            if (fd == -1)
                ErrnoException::throwAsNestedOf<EpollCreateException>();
        }

        inline ~Epoll() noexcept { ::close(fd); }

    /* Fields: */

        int const fd;

    };
    #endif

public: /* Methods: */

    inline EventLoop() {
        #if defined(__linux__)
        epollCtl<EPOLL_CTL_ADD>(m_closePipe.readEnd, INPUT_EVENTS, nullptr);
        #endif
    }

    inline ~EventLoop() noexcept { stop(); }

    template <typename F>
    static std::unique_ptr<EventHandler> createHandler(F && f) {
        struct TempHandler final: EventLoop::EventHandler {
            inline TempHandler(F && f_)
                : m_f{std::forward<F>(f_)}
            {}

            void handleEvents(EventLoop::EventSet const events)
                    noexcept final override
            { m_f(events); }

            F m_f;
        };
        return std::unique_ptr<EventHandler>(
                    new TempHandler(std::forward<F>(f)));
    }

    template <typename F>
    static std::unique_ptr<EventHandler> createSimpleHandler(F && f)
    { return createHandler([f](EventLoop::EventSet const) noexcept { f(); }); }

    inline void insertDisabled(int const fd) {
        #if defined(__linux__)
        disabledInsertOrModify<void, &EventLoop::epollCtl<EPOLL_CTL_ADD> >(fd);
        #endif
    }

    inline void modify(int const fd,
                       EventSet const events,
                       EventHandler & handler)
    {
        assert(&handler);
        #if defined(__linux__)
        epollCtl<EPOLL_CTL_MOD>(fd, events | EPOLLONESHOT, &handler);
        #endif
    }

    inline void disable(int const fd) {
        #if defined(__linux__)
        disabledInsertOrModify<void, &EventLoop::epollCtl<EPOLL_CTL_MOD> >(fd);
        #endif
    }

    inline bool insert(int const fd,
                       EventSet const events,
                       EventHandler & handler)
    {
        assert(&handler);
        #if defined(__linux__)
        return epollInsert(fd, events | EPOLLONESHOT, &handler);
        #endif
    }

    inline bool insertOrModify(int const fd,
                               EventSet const events,
                               EventHandler & handler)
    {
        assert(&handler);
        #if defined(__linux__)
        return epollInsertOrModify(fd, events | EPOLLONESHOT, &handler);
        #endif
    }

    inline bool insertDisabledOrDisable(int const fd)
    {
        #if defined(__linux__)
        return disabledInsertOrModify<bool,
                                      &EventLoop::epollInsertOrModify>(fd);
        #endif
    }

    inline bool remove(int const fd) {
        #if defined(__linux__)
        return epollRemove(fd);
        #endif
    }

    inline void spinOnLoopIterationEnd() noexcept {
        auto const start = m_loopCounter.load(std::memory_order_acquire);
        if (m_stopMutex.try_lock()) {
            m_stopMutex.unlock();
            return;
        }
        if (m_loopCounter.load(std::memory_order_acquire) != start)
            return;
        static char const data = '\0';
        for (;;) {
            auto const w = ::write(m_closePipe.writeEnd, &data, 1u);
            assert(w <= 1u);
            if (w > 0)
                break;
            assert(w == -1);
            #if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
            assert(errno == EAGAIN
                   || errno == EINTR
                   || errno == EWOULDBLOCK
                   || errno == ENOSPC
                   || errno == EIO
                   || errno == ENOBUFS);
            #else
            assert(errno == EAGAIN
                   || errno == EINTR
                   || errno == ENOSPC
                   || errno == EIO
                   || errno == ENOBUFS);
            #endif
        }
        while (m_loopCounter.load(std::memory_order_acquire) == start)
            sharemind::spinWait();
    }

    inline void stopAsync() noexcept {
        m_closePipe.writeEndClose();
    }

    inline void stop() noexcept {
        stopAsync();
        // Block until run() exits:
        std::lock_guard<std::mutex> const guard{m_stopMutex};
    }

    inline void run() {
        std::lock_guard<std::mutex> const guard{m_stopMutex};
        #if defined(__linux__)

        using sharemind::ErrnoException;
        ::epoll_event events[DEFAULT_MAX_EVENTS];

        SHAREMIND_SCOPE_EXIT(loopIterationFinish());
        for (;;) {
            auto const numEvents = ::epoll_wait(m_epoll.fd,
                                                events,
                                                DEFAULT_MAX_EVENTS,
                                                DEFAULT_EPOLL_TIMEOUT_MS);
            assert(numEvents <= numEvents);
            if (numEvents < 0) {
                assert(numEvents == -1);
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                ErrnoException::throwAsNestedOf<EpollWaitException>();
            }

            auto const eventsEnd = events + numEvents;
            for (auto const * it = events; it < eventsEnd; ++it) {
                if (it->data.ptr == nullptr) {
                    // Signal received, handle remaining events before handling:
                    auto const & signalEvent = *it;
                    while (++it < eventsEnd) {
                        assert(it->data.ptr != nullptr);
                        handleEvent(*it);
                    }
                    if (signalEvent.events & (ALL_HUP_EVENTS | ERROR_EVENTS))
                        /* Error or stop signalled, handle remaining events and
                         * return: */
                        return;
                    assert(signalEvent.events & INPUT_DATA_EVENTS);
                    // Flush pipe:
                    constexpr static std::size_t const DISCARD_SIZE =
                            sizeof(::epoll_event) * DEFAULT_MAX_EVENTS;
                    for (;;) {
                        auto const rd = ::read(m_closePipe.readEnd,
                                               events,
                                               DISCARD_SIZE);
                        assert(static_cast<std::size_t>(rd) <= DISCARD_SIZE);
                        if (rd <= 0) {
                            if (rd == 0 // HUP or EOF
                                    || (assert(rd == -1),
                                        (errno != EAGAIN) && (errno != EINTR)))
                                return; // Error or stop signalled
                        } else if (static_cast<std::size_t>(rd) < DISCARD_SIZE)
                        { goto Sharemind_EventLoop_run_continue; }
                    }
                }
                handleEvent(*it);
            }
    Sharemind_EventLoop_run_continue:
            loopIterationFinish();
        }
        #endif
    }

    inline void spinUntilLoopIterationEnd() noexcept
    { spinOnLoopIterationEnd_(&sharemind::spinWait); }

    inline void yieldUntilLoopIterationEnd() noexcept
    { spinOnLoopIterationEnd_(&std::this_thread::yield); }

private: /* Methods: */

    inline void loopIterationFinish() noexcept
    { m_loopCounter.fetch_add(1u, std::memory_order_release); }

    inline void handleEvent(::epoll_event const & event) noexcept
    { static_cast<EventHandler *>(event.data.ptr)->handleEvents(event.events); }

    template <typename SpinFunction>
    inline void spinOnLoopIterationEnd_(SpinFunction spinFunction) noexcept {
        auto const start = m_loopCounter.load(std::memory_order_acquire);
        if (m_stopMutex.try_lock()) {
            m_stopMutex.unlock();
            return;
        }
        if (m_loopCounter.load(std::memory_order_acquire) != start)
            return;
        static char const data = '\0';
        for (;;) {
            auto const w = ::write(m_closePipe.writeEnd, &data, 1u);
            assert(w <= 1);
            if (w > 0)
                break;
            assert(w == -1);
            if (errno != EAGAIN
                && errno != EINTR
                #if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
                && errno != EWOULDBLOCK
                #endif
                && errno != ENOSPC
                && errno != EIO
                && errno != ENOBUFS)
                return;
        }
        while (m_loopCounter.load(std::memory_order_acquire) == start)
            spinFunction();
    }

    #if defined(__linux__)
    static EventHandler & nullHandler() noexcept {
        static struct NullHandler final: EventHandler {
            void handleEvents(EventSet) noexcept final override {}
        } nullHandler;
        return nullHandler;
    }

    template <typename R,
              R (EventLoop::*f)(int const,
                                EventSet const,
                                EventHandler * const)>
    inline R disabledInsertOrModify(int const fd)
    { return (this->*f)(fd, EPOLLONESHOT, &nullHandler()); }

    template <int OP>
    inline void epollCtl(int const fd,
                         EventSet const events,
                         EventHandler * const handler)
    {
        ::epoll_event e;
        e.events = events;
        e.data.ptr = handler;
        syscallLoop<EpollCtlException>(
                    ::epoll_ctl,
                    [](int const r) noexcept { return r == 0; },
                    m_epoll.fd,
                    OP,
                    fd,
                    &e);
    }

    inline bool epollRemove(int const fd) {
        while (::epoll_ctl(m_epoll.fd, EPOLL_CTL_DEL, fd, nullptr) != 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            if (errno == ENOENT)
                return false;
            sharemind::ErrnoException::throwAsNestedOf<EpollCtlException>();
        }
        return true;
    }

    inline bool epollInsert(int const fd,
                            EventSet const events,
                            EventHandler * const handler)
    {
        ::epoll_event e;
        e.events = events;
        e.data.ptr = handler;
        while (::epoll_ctl(m_epoll.fd, EPOLL_CTL_ADD, fd, &e) != 0) {
            if (errno == EEXIST)
                return false;
            if (errno != EAGAIN && errno != EINTR)
                sharemind::ErrnoException::throwAsNestedOf<EpollCtlException>();
        }
        return true;
    }

    inline bool epollInsertOrModify(int const fd,
                                    EventSet const events,
                                    EventHandler * const handler)
    {
        ::epoll_event e;
        e.events = events;
        e.data.ptr = handler;
        for (;;) {
            auto const r = ::epoll_ctl(m_epoll.fd, EPOLL_CTL_ADD, fd, &e);
            if (r == 0)
                return true;
            assert(r == -1);
            if (errno == EEXIST)
                break;
            if (errno != EAGAIN && errno != EINTR)
                sharemind::ErrnoException::throwAsNestedOf<EpollCtlException>();
        }
        syscallLoop<EpollCtlException>(
                    ::epoll_ctl,
                    [](int const r) noexcept { return r == 0; },
                    m_epoll.fd,
                    EPOLL_CTL_MOD,
                    fd,
                    &e);
        return false;
    }
    #endif

private: /* Fields: */

    #if defined(__linux__)
    Epoll const m_epoll;
    #endif
    Pipe m_closePipe;

    /* Initialization required for valgrind not to report uninitialized memory
       access: */
    std::atomic<unsigned> m_loopCounter{0u};
    std::mutex m_stopMutex;

}; /* class EventLoop { */

} /* namespace sharemind {*/

#endif /* SHAREMIND_EVENTLOOP_H */
