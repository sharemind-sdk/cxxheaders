/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_FILEDESCRIPTOR_H
#define SHAREMIND_FILEDESCRIPTOR_H

#include <fcntl.h>
#include <unistd.h>
#include <sharemind/Posix.h>
#include "detail/ExceptionMacros.h"
#include "Exception.h"


namespace sharemind {

class FileDescriptor {

    SHAREMIND_DETAIL_DEFINE_EXCEPTION(sharemind::Exception, Exception);
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            FailedToDuplicateException,
            "Failed to duplicate file descriptor!");

public: /* Methods: */

    FileDescriptor(FileDescriptor && move) noexcept
        : m_fd(move.m_fd)
    { move.m_fd = -1; }

    FileDescriptor(FileDescriptor const &) = delete;

    FileDescriptor(int const fd = -1) noexcept : m_fd(fd) {}

    ~FileDescriptor() noexcept { close(); }

    FileDescriptor & operator=(FileDescriptor && move) noexcept {
        if (&move != this) {
            close();
            m_fd = move.m_fd;
            move.m_fd = -1;
        }
        return *this;
    }

    FileDescriptor & operator=(FileDescriptor const &) = delete;

    bool valid() const noexcept {
        /* POSIX defines a file descriptor as a non-negative integer. It is not
           possible to check the upper limit against OPEN_MAX, because it may
           change during the execution of the process, hence one might end up
           with valid file descriptors which are greater than or equal to
           OPEN_MAX. */
        return m_fd >= 0;
    }

    FileDescriptor duplicate() const {
        return syscallLoop<FailedToDuplicateException>(
                    fcntl,
                    [](int const r) { return r != -1; },
                    nativeHandle(),
                    F_DUPFD_CLOEXEC,
                    0);
    }

    void reset(int const fd) noexcept {
        if (m_fd != fd) {
            close();
            m_fd = fd;
        }
    }

    int release() noexcept {
        int const r = m_fd;
        m_fd = -1;
        return r;
    }

    void close() noexcept {
        if (m_fd != -1) {
            ::close(m_fd);
            m_fd = -1;
        }
    }

    int nativeHandle() const noexcept { return m_fd; }

private: /* Fields: */

    int m_fd;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_FILEDESCRIPTOR_H */
