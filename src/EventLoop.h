/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_EVENTLOOP_H
#define SHAREMIND_EVENTLOOP_H

#include <atomic>
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <utility>
#include "detail/ExceptionMacros.h"
#include "Exception.h"
#include "Posix.h"
#include "ScopeExit.h"
#include "Spinwait.h"
#include "ThrowNested.h"

#if defined(__linux__)
#include <sys/epoll.h>
#else
#error Detected an operating system which is currently not supported!
#endif


namespace sharemind {

class EventLoop {

public: /* Constants: */

    static constexpr std::size_t DEFAULT_MAX_EVENTS = 128u;
    static_assert(DEFAULT_MAX_EVENTS <= std::numeric_limits<int>::max(), "");
    static constexpr int DEFAULT_EPOLL_TIMEOUT_MS = 50;

    #if defined(__linux__)
    using EventSet = std::uint32_t;
    static constexpr EventSet COMMON_ERROR_EVENTS = EPOLLERR;
    static constexpr EventSet INPUT_ERROR_EVENTS = COMMON_ERROR_EVENTS;
    static constexpr EventSet OUTPUT_ERROR_EVENTS = COMMON_ERROR_EVENTS;
    static constexpr EventSet ALL_ERROR_EVENTS =
            INPUT_ERROR_EVENTS | OUTPUT_ERROR_EVENTS;

    static constexpr EventSet COMMON_HUP_EVENTS = EPOLLHUP;
    static constexpr EventSet INPUT_HUP_EVENTS =
            COMMON_HUP_EVENTS | EPOLLRDHUP;
    static constexpr EventSet OUTPUT_HUP_EVENTS = COMMON_HUP_EVENTS;
    static constexpr EventSet ALL_HUP_EVENTS =
            INPUT_HUP_EVENTS | OUTPUT_HUP_EVENTS;

    static constexpr EventSet COMMON_FATAL_EVENTS =
            COMMON_ERROR_EVENTS | COMMON_HUP_EVENTS;
    static constexpr EventSet INPUT_FATAL_EVENTS =
            INPUT_ERROR_EVENTS | INPUT_HUP_EVENTS;
    static constexpr EventSet OUTPUT_FATAL_EVENTS =
            OUTPUT_ERROR_EVENTS | OUTPUT_HUP_EVENTS;
    static constexpr EventSet ALL_FATAL_EVENTS =
            ALL_ERROR_EVENTS | ALL_HUP_EVENTS;

    static constexpr EventSet COMMON_DATA_EVENTS = 0u;
    static constexpr EventSet INPUT_DATA_EVENTS =
            COMMON_DATA_EVENTS | EPOLLPRI | EPOLLIN;
    static constexpr EventSet OUTPUT_DATA_EVENTS =
            COMMON_DATA_EVENTS | EPOLLOUT;
    static constexpr EventSet ALL_DATA_EVENTS =
            INPUT_DATA_EVENTS | OUTPUT_DATA_EVENTS;

    static constexpr EventSet ALL_INPUT_EVENTS =
            INPUT_DATA_EVENTS | INPUT_FATAL_EVENTS;
    static constexpr EventSet ALL_OUTPUT_EVENTS =
            OUTPUT_DATA_EVENTS | OUTPUT_FATAL_EVENTS;
    static constexpr EventSet ALL_EVENTS = ALL_DATA_EVENTS | ALL_FATAL_EVENTS;
    #endif

public: /* Types: */

    SHAREMIND_DETAIL_DEFINE_EXCEPTION(sharemind::Exception, Exception);

    #if defined(__linux__) || defined(__NetBSD__) \
        || defined(__OpenBSD__) || defined(__FreeBSD__)
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                PipeCreateException,
                                                "pipe2() failed!");
    #else
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                PipeCreateException,
                                                "pipe() failed!");
    #endif
    #if defined(__linux__)
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                EpollCreateException,
                                                "epoll_create1() failed!");
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                EpollCtlException,
                                                "epoll_ctl() failed!");
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                EpollWaitException,
                                                "epoll_wait() failed!");
    #else
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(Exception,
                                                FcntlException,
                                                "fcntl() failed!");
    #endif

    struct EventHandler {
        virtual ~EventHandler() noexcept {}
        virtual void handleEvents(EventSet events) noexcept = 0;
    };

private: /* Types: */

    struct Pipe {

    /* Methods: */

        Pipe() {
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

        ~Pipe() noexcept {
            ::close(readEnd);
            if (readEnd != writeEnd)
                ::close(writeEnd);
        }

        void writeEndClose() noexcept {
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

        Epoll()
            : fd(::epoll_create1(EPOLL_CLOEXEC))
        {
            using sharemind::ErrnoException;
            if (fd == -1)
                throwNested(ErrnoException(errno), EpollCreateException());
        }

        ~Epoll() noexcept { ::close(fd); }

    /* Fields: */

        int const fd;

    };
    #endif

public: /* Methods: */

    EventLoop() {
        #if defined(__linux__)
        epollCtl<EPOLL_CTL_ADD>(m_closePipe.readEnd, ALL_INPUT_EVENTS, nullptr);
        #endif
    }

    ~EventLoop() noexcept { stop(); }

    template <typename F>
    static std::unique_ptr<EventHandler> createHandler(F && f) {
        struct TempHandler final: EventLoop::EventHandler {
            TempHandler(F && f_) : m_f{std::forward<F>(f_)} {}

            void handleEvents(EventLoop::EventSet const events)
                    noexcept final override
            { m_f(events); }

            F m_f;
        };
        return std::unique_ptr<EventHandler>(
                    new TempHandler(std::forward<F>(f)));
    }

    template <typename F>
    static std::unique_ptr<EventHandler> createSimpleHandler(F && f) {
        static_assert(noexcept(f()), "");
        return createHandler([f](EventLoop::EventSet const) mutable noexcept
                             { f(); });
    }

    void insertDisabled(int const fd) {
        #if defined(__linux__)
        disabledInsertOrModify<void, &EventLoop::epollCtl<EPOLL_CTL_ADD> >(fd);
        #endif
    }

    void modify(int const fd, EventSet const events, EventHandler & handler) {
        #if defined(__linux__)
        epollCtl<EPOLL_CTL_MOD>(fd, events | EPOLLONESHOT, &handler);
        #endif
    }

    void disable(int const fd) {
        #if defined(__linux__)
        disabledInsertOrModify<void, &EventLoop::epollCtl<EPOLL_CTL_MOD> >(fd);
        #endif
    }

    bool insert(int const fd, EventSet const events, EventHandler & handler) {
        #if defined(__linux__)
        return epollInsert(fd, events | EPOLLONESHOT, &handler);
        #endif
    }

    bool insertOrModify(int const fd,
                        EventSet const events,
                        EventHandler & handler)
    {
        #if defined(__linux__)
        return epollInsertOrModify(fd, events | EPOLLONESHOT, &handler);
        #endif
    }

    bool insertDisabledOrDisable(int const fd) {
        #if defined(__linux__)
        return disabledInsertOrModify<bool,
                                      &EventLoop::epollInsertOrModify>(fd);
        #endif
    }

    bool remove(int const fd) {
        #if defined(__linux__)
        return epollRemove(fd);
        #endif
    }

    void stopAsync() noexcept { m_closePipe.writeEndClose(); }

    void stop() noexcept {
        stopAsync();
        // Block until run() exits:
        std::lock_guard<std::mutex> const guard{m_stopMutex};
    }

    void run() {
        std::lock_guard<std::mutex> const guard{m_stopMutex};
        #if defined(__linux__)

        using sharemind::ErrnoException;
        ::epoll_event events[DEFAULT_MAX_EVENTS];

        SHAREMIND_SCOPE_EXIT(loopIterationFinish());
        for (;;) {
            auto const numEvents =
                    ::epoll_wait(m_epoll.fd,
                                 events,
                                 static_cast<int>(DEFAULT_MAX_EVENTS),
                                 DEFAULT_EPOLL_TIMEOUT_MS);
            if (numEvents < 0) {
                assert(numEvents == -1);
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                throwNested(ErrnoException(errno), EpollWaitException());
            }
            assert(numEvents <= static_cast<int>(DEFAULT_MAX_EVENTS));

            auto const eventsEnd = events + numEvents;
            for (auto const * it = events; it < eventsEnd; ++it) {
                if (it->data.ptr == nullptr) {
                    // Signal received, handle remaining events before handling:
                    auto const & signalEvent = *it;
                    while (++it < eventsEnd) {
                        assert(it->data.ptr != nullptr);
                        handleEvent(*it);
                    }
                    if (signalEvent.events & ALL_FATAL_EVENTS)
                        /* Error or stop signalled, handle remaining events and
                         * return: */
                        return;
                    assert(signalEvent.events & INPUT_DATA_EVENTS);
                    // Flush pipe:
                    constexpr static std::size_t DISCARD_SIZE =
                            sizeof(::epoll_event) * DEFAULT_MAX_EVENTS;
                    for (;;) {
                        auto const rd = ::read(m_closePipe.readEnd,
                                               events,
                                               DISCARD_SIZE);
                        assert(static_cast<std::size_t>(rd) <= DISCARD_SIZE);
                        if (rd <= 0) {
                            if (rd == 0 // HUP or EOF
                                    || (static_cast<void>(assert(rd == -1)),
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

    void spinUntilLoopIterationEnd() noexcept
    { spinOnLoopIterationEnd_(&sharemind::spinWait); }

    void yieldUntilLoopIterationEnd() noexcept
    { spinOnLoopIterationEnd_(&std::this_thread::yield); }

private: /* Methods: */

    void loopIterationFinish() noexcept
    { m_loopCounter.fetch_add(1u, std::memory_order_release); }

    void handleEvent(::epoll_event const & event) noexcept
    { static_cast<EventHandler *>(event.data.ptr)->handleEvents(event.events); }

    template <typename SpinFunction>
    void spinOnLoopIterationEnd_(SpinFunction spinFunction) noexcept {
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
    R disabledInsertOrModify(int const fd)
    { return (this->*f)(fd, EPOLLONESHOT, &nullHandler()); }

    template <int OP>
    void epollCtl(int const fd,
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

    bool epollRemove(int const fd) {
        while (::epoll_ctl(m_epoll.fd, EPOLL_CTL_DEL, fd, nullptr) != 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            if (errno == ENOENT)
                return false;
            throwNested(ErrnoException(errno), EpollCtlException());
        }
        return true;
    }

    bool epollInsert(int const fd,
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
                throwNested(ErrnoException(errno), EpollCtlException());
        }
        return true;
    }

    bool epollInsertOrModify(int const fd,
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
                throwNested(ErrnoException(errno), EpollCtlException());
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

} /* namespace sharemind { */

#endif /* SHAREMIND_EVENTLOOP_H */
