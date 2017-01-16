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

#include "../src/Future.h"

#include <chrono>
#include <thread>
#include "../src/TestAssert.h"


using sharemind::BrokenPromiseException;
using sharemind::Future;
using sharemind::Promise;

class DelayedThread: public std::thread {

public: /* Methods: */

    template <typename F>
    DelayedThread(F f)
        : m_thread([f]() noexcept(noexcept(f())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return f();
        })
    {}

    ~DelayedThread() noexcept { m_thread.join(); }

private: /* Fields: */

    std::thread m_thread;

};

class SetReadyDelayedThread final: public DelayedThread {

public: /* Methods: */

    template <typename Promise>
    SetReadyDelayedThread(std::shared_ptr<Promise> promise)
        : DelayedThread([promise]() noexcept { promise->setReady(); })
    {}

};

class SetValueDelayedThread final: public DelayedThread {

public: /* Methods: */

    template <typename Promise, typename Value>
    SetValueDelayedThread(std::shared_ptr<Promise> promise, Value value)
        : DelayedThread([promise, value] () noexcept { promise->setValue(value); })
    {}

};

class SetExceptionDelayedThread final: public DelayedThread {

public: /* Methods: */

    template <typename Promise, typename Exception>
    SetExceptionDelayedThread(std::shared_ptr<Promise> promise,
                              Exception exception)
        : DelayedThread(
                [promise, exception]() noexcept
                { promise->setException(exception); })
    {}

};

struct E { int c; };
struct V { int v; };

template <typename T> struct Smth { T v; };
template <> struct Smth<void> {};

template <typename T>
void testTypeAgnostic() noexcept {
    static_assert(std::is_default_constructible<Future<T> >::value, "");
    static_assert(!std::is_copy_constructible<Future<T> >::value, "");
    static_assert(std::is_nothrow_move_constructible<Future<T> >::value, "");
    static_assert(std::is_nothrow_destructible<Future<T> >::value, "");
    static_assert(!std::is_copy_assignable<Future<T> >::value, "");
    static_assert(std::is_nothrow_move_assignable<Future<T> >::value, "");

    static_assert(std::is_default_constructible<Promise<T> >::value, "");
    static_assert(!std::is_copy_constructible<Promise<T> >::value, "");
    static_assert(std::is_nothrow_move_constructible<Promise<T> >::value, "");
    static_assert(std::is_nothrow_destructible<Promise<T> >::value, "");
    static_assert(!std::is_copy_assignable<Promise<T> >::value, "");
    static_assert(std::is_nothrow_move_assignable<Promise<T> >::value, "");

    static_assert(std::is_same<decltype(Promise<T>().takeFuture()), Future<T> >::value, "");
    static_assert(std::is_same<decltype(Promise<T>().setException(std::declval<std::exception_ptr>())), void>::value, "");

    {
        std::unique_ptr<Promise<T> > pp(new Promise<T>());
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(pp->haveFuture());
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(!pp->haveFuture());
        SHAREMIND_TESTASSERT(f.isValid());
        pp.reset();
        SHAREMIND_TESTASSERT(f.isValid());
        try {
            f.takeValue();
            SHAREMIND_TEST_UNREACHABLE;
        } catch (BrokenPromiseException const &) {
            SHAREMIND_TESTASSERT(!f.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }
    {
        std::unique_ptr<Promise<T> > pp(new Promise<T>());
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(pp->haveFuture());
        static_cast<void>(pp->takeFuture()); // Drop
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(!pp->haveFuture());
    }
    {
        Promise<T> p;
        p.setException(std::make_exception_ptr(E()));
        auto f(p.takeFuture());
        try {
            f.takeValue();
            SHAREMIND_TEST_UNREACHABLE;
        } catch (E const &) {
            SHAREMIND_TESTASSERT(!f.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }
    {
        Promise<T> p;
        p.setException(std::make_exception_ptr(E()));
        auto f(p.takeFuture().then(
                   [](Future<T>) { return Future<Smth<T> >{}; }));
        static_assert(std::is_same<decltype(f), Future<Smth<T> > >::value, "");
        assert(f.isValid());
        try {
            f.takeValue();
            SHAREMIND_TEST_UNREACHABLE;
        } catch (BrokenPromiseException const &) {
            SHAREMIND_TESTASSERT(!f.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }
    {
        struct E2 {};
        auto spp(std::make_shared<Promise<Smth<T> > >());
        spp->setException(E2());
        Promise<T> p;
        p.setException(std::make_exception_ptr(E()));
        auto f(p.takeFuture().then(
                   [spp](Future<T>) { return spp->takeFuture(); }));
        assert(spp.unique());
        spp.reset();
        static_assert(std::is_same<decltype(f), Future<Smth<T> > >::value, "");
        assert(f.isValid());
        try {
            f.takeValue();
            static_assert(std::is_same<decltype(f.takeValue()), Smth<T> >::value, "");
            SHAREMIND_TEST_UNREACHABLE;
        } catch (E2 const &) {
            SHAREMIND_TESTASSERT(!f.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }
}

int main() {
    testTypeAgnostic<void>();
    testTypeAgnostic<V>();

    // Future getting ready without waiting:
    {
        std::unique_ptr<Promise<void> > pp(new Promise<void>);
        auto f = pp->takeFuture();
        pp->setReady();
        SHAREMIND_TESTASSERT(!pp->isValid());
        SHAREMIND_TESTASSERT(!pp->haveFuture());
        pp.reset();

        static_assert(std::is_same<decltype(f.takeValue()), void>::value, "");
        f.takeValue();

        SHAREMIND_TESTASSERT(!f.isValid());
    }{
        std::unique_ptr<Promise<V> > pp(new Promise<V>);
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(pp->haveFuture());

        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(!pp->haveFuture());
        SHAREMIND_TESTASSERT(f.isValid());

        pp->setValue(V{42});
        SHAREMIND_TESTASSERT(!pp->isValid());

        pp.reset();
        SHAREMIND_TESTASSERT(f.isValid());

        static_assert(std::is_same<decltype(f.takeValue()), V>::value, "");
        SHAREMIND_TESTASSERT(f.takeValue().v == 42);
        SHAREMIND_TESTASSERT(!f.isValid());
    }

    // Future getting ready with waiting:
    {
        auto pp = std::make_shared<Promise<void> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(!pp->haveFuture());
        SHAREMIND_TESTASSERT(f.isValid());

        SetReadyDelayedThread(std::move(pp));
        static_assert(std::is_same<decltype(f.takeValue()), void>::value, "");
        f.takeValue();
        SHAREMIND_TESTASSERT(!f.isValid());
    }{
        auto pp = std::make_shared<Promise<V> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(f.isValid());

        SetValueDelayedThread(std::move(pp), V{42});
        SHAREMIND_TESTASSERT(f.takeValue().v == 42);
        SHAREMIND_TESTASSERT(!f.isValid());
    }

    { // .then() without waiting
        auto pp = std::make_shared<Promise<void> >();
        auto f = pp->takeFuture();
        pp->setReady();
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 = f.then([](Future<void> fut){ fut.takeValue(); });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());
        f2.takeValue();
        SHAREMIND_TESTASSERT(!f2.isValid());
    }{
        auto pp = std::make_shared<Promise<V> >();
        auto f = pp->takeFuture();
        pp->setValue(V{42});
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 = f.then([](Future<V> fut){ SHAREMIND_TESTASSERT(fut.takeValue().v == 42); });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());
        f2.takeValue();
        SHAREMIND_TESTASSERT(!f2.isValid());
    }

    { // .then() with first future returning value, second returning value
        auto pp = std::make_shared<Promise<void> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 =
                f.then([](Future<void> fut){ fut.takeValue(); return V{42}; });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());

        SetReadyDelayedThread(std::move(pp));
        SHAREMIND_TESTASSERT(f2.takeValue().v == 42);
        SHAREMIND_TESTASSERT(!f2.isValid());
    }{
        auto pp = std::make_shared<Promise<V> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 = f.then([](Future<V> fut){ return fut.takeValue(); });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());

        SetValueDelayedThread(std::move(pp), V{42});
        SHAREMIND_TESTASSERT(f2.takeValue().v == 42);
        SHAREMIND_TESTASSERT(!f2.isValid());
    }

    { // .then() with first future returning value, second returning exception
        auto pp = std::make_shared<Promise<void> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 =
                f.then(
                    [](Future<void> fut) -> V {
                        fut.takeValue();
                        throw E();
                    });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());

        SetReadyDelayedThread(std::move(pp));
        try {
            f2.takeValue();
        } catch (E const &) {
            SHAREMIND_TESTASSERT(!f2.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }{
        auto pp = std::make_shared<Promise<V> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 =
                f.then(
                    [](Future<V> fut) -> V {
                        SHAREMIND_TESTASSERT(fut.takeValue().v == 42);;
                        throw E();
                    });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());

        SetValueDelayedThread(std::move(pp), V{42});
        try {
            f2.takeValue();
        } catch (E const &) {
            SHAREMIND_TESTASSERT(!f2.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }

    { // .then() with first future returning excption, second wrapping it:
        auto pp = std::make_shared<Promise<void> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 =
                f.then(
                    [](Future<void> fut) -> V {
                        try {
                            fut.takeValue();
                            SHAREMIND_TEST_UNREACHABLE;
                        } catch (E const & e) {
                            SHAREMIND_TESTASSERT(e.c == 0);
                            SHAREMIND_TESTASSERT(!fut.isValid());
                            throw E{42};
                        } catch (...) {
                            SHAREMIND_TEST_UNREACHABLE;
                        }
                    });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());

        SetExceptionDelayedThread(std::move(pp), E{0});
        try {
            f2.takeValue();
        } catch (E const & e) {
            SHAREMIND_TESTASSERT(e.c == 42);
            SHAREMIND_TESTASSERT(!f2.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }{
        auto pp = std::make_shared<Promise<V> >();
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(f.isValid());
        auto f2 =
                f.then(
                    [](Future<V> fut) -> V {
                        try {
                            fut.takeValue();
                            SHAREMIND_TEST_UNREACHABLE;
                        } catch (E const & e) {
                            SHAREMIND_TESTASSERT(e.c == 0);
                            SHAREMIND_TESTASSERT(!fut.isValid());
                            throw E{42};
                        } catch (...) {
                            SHAREMIND_TEST_UNREACHABLE;
                        }
                    });
        SHAREMIND_TESTASSERT(!f.isValid());
        SHAREMIND_TESTASSERT(f2.isValid());

        SetExceptionDelayedThread(std::move(pp), E{0});
        try {
            f2.takeValue();
        } catch (E const & e) {
            SHAREMIND_TESTASSERT(e.c == 42);
            SHAREMIND_TESTASSERT(!f2.isValid());
        } catch (...) {
            SHAREMIND_TEST_UNREACHABLE;
        }
    }
}
