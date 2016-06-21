/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_SINGLETHREADEVENTLOOP_H
#define SHAREMIND_SINGLETHREADEVENTLOOP_H

#include "EventLoop.h"
#include <thread>


namespace sharemind {

class SingleThreadEventLoop: public EventLoop {

public: /* Methods: */

    inline SingleThreadEventLoop()
        : m_thread{
            [this]() noexcept {
                try {
                    this->run();
                } catch (...) {}
            }}
    {}

    template <typename ExceptionHandler>
    inline SingleThreadEventLoop(ExceptionHandler && exceptionHandler)
        : m_thread{
            [this,exceptionHandler]() noexcept {
                try {
                    this->run();
                } catch (...) {
                    static_assert(noexcept(exceptionHandler()), "");
                    exceptionHandler();
                }
            }}
    {}

    inline ~SingleThreadEventLoop() noexcept {
        if (m_thread.joinable()) {
            this->stop();
            m_thread.join();
        }
    }

private: /* Fields: */

    std::thread m_thread;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_SINGLETHREADEVENTLOOP_H */
