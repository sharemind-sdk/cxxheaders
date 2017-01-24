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

#include "../src/TimeoutsThread.h"

#include <future>
#include <iostream>
#include "../src/TestAssert.h"


int main() {
    std::mutex mutex;
    std::string outStr;
    auto const output(
                [&outStr, &mutex](char const c) noexcept {
                    std::lock_guard<std::mutex> const guard(mutex);
                    outStr.push_back(c);
                });
    auto t1(TimeoutsThread::createTask([&output]() noexcept { output('1'); }));
    auto t2(TimeoutsThread::createTask([&output]() noexcept { output('2'); }));
    auto t3(TimeoutsThread::createTask([&output]() noexcept { output('3'); }));

    auto ta(TimeoutsThread::createTask([&output]() noexcept { output('a'); }));
    auto tb(TimeoutsThread::createTask([&output]() noexcept { output('b'); }));
    auto tc(TimeoutsThread::createTask([&output]() noexcept { output('c'); }));
    auto td(TimeoutsThread::createTask([&output]() noexcept { output('d'); }));
    auto te(TimeoutsThread::createTask([&output]() noexcept { output('e'); }));
    auto tf(TimeoutsThread::createTask([&output]() noexcept { output('f'); }));

    std::promise<void> pr;
    auto f(pr.get_future());
    auto tw(TimeoutsThread::createTask(
                [&pr,&output]() noexcept {
                    output('w');
                    pr.set_value();
                }));

    TimeoutsThread thread;
    auto const start(TimeoutsThread::Clock::now());
    thread.addTimeoutTask(std::chrono::milliseconds(400), std::move(ta));
    thread.addTimeoutTask(std::chrono::milliseconds(400), std::move(tb));
    thread.addTimeoutTask(std::chrono::milliseconds(400), std::move(tc));
    thread.addTimeoutTask(std::chrono::milliseconds(500), std::move(tw));
    thread.addTimeoutTask(std::chrono::milliseconds(400), std::move(td));
    thread.addTimeoutTask(std::chrono::milliseconds(400), std::move(te));
    thread.addTimeoutTask(std::chrono::milliseconds(400), std::move(tf));
    thread.addTimeoutTask(std::chrono::milliseconds(300), std::move(t3));
    thread.addTimeoutTask(std::chrono::milliseconds(200), std::move(t2));
    thread.addTimeoutTask(std::chrono::milliseconds(100), std::move(t1));
    f.get();
    auto const timeTaken(TimeoutsThread::Clock::now() - start);

    SHAREMIND_TESTASSERT(outStr == "123abcdefw");
    SHAREMIND_TESTASSERT(timeTaken >= std::chrono::milliseconds(500));

    // Let's hope our test platform is fast enough:
    SHAREMIND_TESTASSERT(timeTaken < std::chrono::milliseconds(600));
}
