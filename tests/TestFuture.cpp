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
#include "../src/MakeUnique.h"
#include "../src/TestAssert.h"


using sharemind::BrokenPromiseException;
using sharemind::Future;
using sharemind::PackagedTask;
using sharemind::Promise;
using sharemind::makeExceptionalFuture;
using sharemind::makeUnique;


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

template <typename R>
struct TestContinuation {
    template <typename T>
    R operator()(T &&) {
        static_assert(std::is_rvalue_reference<T &&>::value, "");
        return R();
    }
};

template <typename T>
void testTypeAgnostic() noexcept {
    static_assert(std::is_nothrow_default_constructible<Future<T> >::value, "");
    static_assert(!std::is_copy_constructible<Future<T> >::value, "");
    static_assert(std::is_nothrow_move_constructible<Future<T> >::value, "");
    static_assert(std::is_nothrow_destructible<Future<T> >::value, "");
    static_assert(!std::is_copy_assignable<Future<T> >::value, "");
    static_assert(std::is_nothrow_move_assignable<Future<T> >::value, "");
    static_assert(std::is_same<decltype(std::declval<Future<T> &>().takeValue()), T>::value, "");
    static_assert(!noexcept(std::declval<Future<T> &>().takeValue()), "");
    static_assert(std::is_same<decltype(std::declval<Future<T> &>().isValid()), bool>::value, "");
    static_assert(noexcept(std::declval<Future<T> &>().isValid()), "");
    static_assert(std::is_same<decltype(std::declval<Future<T> &>().swap(std::declval<Future<T> &>())), void>::value, "");
    static_assert(noexcept(std::declval<Future<T> &>().swap(std::declval<Future<T> &>())), "");
    static_assert(std::is_same<decltype(std::declval<Future<T> &>().then(std::declval<TestContinuation<void> &>())), Future<void> >::value, "");
    {
        struct X {};
        static_assert(std::is_same<decltype(std::declval<Future<T> &>().then(std::declval<TestContinuation<X> &>())), Future<X> >::value, "");
    }

    static_assert(std::is_default_constructible<Promise<T> >::value, "");
    static_assert(!std::is_copy_constructible<Promise<T> >::value, "");
    static_assert(std::is_nothrow_move_constructible<Promise<T> >::value, "");
    static_assert(std::is_nothrow_destructible<Promise<T> >::value, "");
    static_assert(!std::is_copy_assignable<Promise<T> >::value, "");
    static_assert(std::is_nothrow_move_assignable<Promise<T> >::value, "");
    static_assert(std::is_same<decltype(std::declval<Promise<T> &>().setException(std::declval<E>())), void>::value, "");
    static_assert(noexcept(std::declval<Promise<T> &>().setException(std::declval<E>())), "");
    static_assert(std::is_same<decltype(std::declval<Promise<T> &>().setException(std::declval<std::exception_ptr>())), void>::value, "");
    static_assert(noexcept(std::declval<Promise<T> &>().setException(std::declval<std::exception_ptr>())), "");
    static_assert(std::is_same<decltype(std::declval<Promise<T> &>().isValid()), bool>::value, "");
    static_assert(noexcept(std::declval<Promise<T> &>().isValid()), "");
    static_assert(std::is_same<decltype(std::declval<Promise<T> &>().hasFuture()), bool>::value, "");
    static_assert(noexcept(std::declval<Promise<T> &>().hasFuture()), "");
    static_assert(std::is_same<decltype(std::declval<Promise<T> &>().takeFuture()), Future<T> >::value, "");
    static_assert(noexcept(std::declval<Promise<T> &>().takeFuture()), "");
    static_assert(std::is_same<decltype(std::declval<Promise<T> &>().swap(std::declval<Promise<T> &>())), void>::value, "");
    static_assert(noexcept(std::declval<Promise<T> &>().swap(std::declval<Promise<T> &>())), "");

    {
        auto pp(makeUnique<Promise<T> >());
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(pp->hasFuture());
        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(!pp->hasFuture());
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
        auto pp(makeUnique<Promise<T> >());
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(pp->hasFuture());
        static_cast<void>(pp->takeFuture()); // Drop
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(!pp->hasFuture());
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
        struct X {};
        Promise<T> p;
        p.setException(std::make_exception_ptr(E()));
        auto f(p.takeFuture().then([](Future<T>) { return Future<X>(); }));
        static_assert(std::is_same<decltype(f), Future<X> >::value, "");
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
        struct X {};
        struct E2 {};
        Promise<T> p;
        p.setException(std::make_exception_ptr(E()));
        auto f(p.takeFuture().then(
                   [](Future<T>) { return makeExceptionalFuture<X>(E2()); }));
        static_assert(std::is_same<decltype(f), Future<X> >::value, "");
        assert(f.isValid());
        try {
            f.takeValue();
            static_assert(std::is_same<decltype(f.takeValue()), X>::value, "");
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
        auto pp(makeUnique<Promise<void> >());
        auto f = pp->takeFuture();
        pp->setReady();
        SHAREMIND_TESTASSERT(!pp->isValid());
        SHAREMIND_TESTASSERT(!pp->hasFuture());
        pp.reset();

        static_assert(std::is_same<decltype(f.takeValue()), void>::value, "");
        f.takeValue();

        SHAREMIND_TESTASSERT(!f.isValid());
    }{
        auto pp(makeUnique<Promise<V> >());
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(pp->hasFuture());

        auto f = pp->takeFuture();
        SHAREMIND_TESTASSERT(pp->isValid());
        SHAREMIND_TESTASSERT(!pp->hasFuture());
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
        SHAREMIND_TESTASSERT(!pp->hasFuture());
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

    using PT = PackagedTask<V(V,V)>;
    static_assert(std::is_default_constructible<PT>::value, "");
    static_assert(!std::is_copy_constructible<PT>::value, "");
    static_assert(std::is_nothrow_move_constructible<PT>::value, "");
    static_assert(std::is_nothrow_destructible<PT>::value, "");
    static_assert(!std::is_copy_assignable<PT>::value, "");
    static_assert(std::is_nothrow_move_assignable<PT>::value, "");
    static_assert(noexcept(std::declval<PT &>()(std::declval<V>(), std::declval<V>())), "");
    static_assert(std::is_same<decltype(std::declval<PT &>().isValid()),
                               bool>::value, "");
    static_assert(noexcept(std::declval<PT &>().isValid()), "");
    static_assert(std::is_same<decltype(std::declval<PT &>().takeFuture()),
                               Future<V> >::value, "");
    static_assert(noexcept(std::declval<PT &>().takeFuture()), "");
    static_assert(noexcept(std::declval<PT &>().swap(std::declval<PT &>())), "");

    static_assert(noexcept(std::swap(std::declval<Future<V> &>(),
                                     std::declval<Future<V> &>())), "");
    static_assert(noexcept(std::swap(std::declval<Promise<V> &>(),
                                     std::declval<Promise<V> &>())), "");
    static_assert(noexcept(std::swap(std::declval<PT &>(),
                                     std::declval<PT &>())), "");
}
