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

#ifndef SHAREMIND_FUTURE_H
#define SHAREMIND_FUTURE_H

#include <cassert>
#include <condition_variable>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <new>
#include <type_traits>
#include "Exception.h"
#include "StripReferenceWrapper.h"


namespace sharemind {

template <typename T> class Future;

SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(std::exception,
                                     BrokenPromiseException,
                                     "Promise broken!");

namespace Detail {
namespace Future {

struct ContinuationBase { \
    virtual ~ContinuationBase() noexcept {}
    virtual void run() noexcept = 0;
};

#define SHAREMIND_SHAREDSTATE_COMMON \
    /* Types: */ \
        using ContinuationPtr = \
                std::unique_ptr<Detail::Future::ContinuationBase>; \
    /* Methods: */ \
        void setException(std::exception_ptr e) noexcept { \
            if (auto const continuation = \
                    [this](std::exception_ptr e_) noexcept -> ContinuationPtr {\
                        std::lock_guard<std::mutex> const guard(mutex); \
                        assert(!isSet); \
                        exception = std::move(e_); \
                        isSet = true; \
                        cond.notify_one(); \
                        return std::move(m_continuation); \
                    }(std::move(e))) \
                continuation->run(); \
        } \
        std::unique_lock<std::mutex> waitReadyLock() noexcept { \
            std::unique_lock<std::mutex> lock(mutex); \
            cond.wait(lock, [this]() noexcept { return isSet; }); \
            return lock; \
        } \
        void then(ContinuationPtr continuation) noexcept { \
            if (auto const cont = \
                    [this](ContinuationPtr c) noexcept -> ContinuationPtr { \
                        std::lock_guard<std::mutex> const guard(mutex); \
                        assert(!m_continuation); \
                        if (isSet) \
                            return c; \
                        m_continuation = std::move(c); \
                        return nullptr; \
                    }(std::move(continuation))) \
                cont->run(); \
        } \
    /* Fields: */ \
        std::mutex mutex; \
        std::condition_variable cond; \
        std::exception_ptr exception; \
        ContinuationPtr m_continuation; \
        bool isSet = false;

template <typename T>
struct SharedState {

    static_assert(!std::is_reference<T>::value, "");

    SHAREMIND_SHAREDSTATE_COMMON

/* Methods: */

    template <typename ... Args>
    void emplaceValue(Args && ... args)
            noexcept(noexcept(T(std::forward<Args>(args)...)))
    {
        if (auto const continuation =
                [this](Args && ... args_) noexcept -> ContinuationPtr {
                    std::lock_guard<std::mutex> const guard(mutex);
                    assert(!isSet);
                    new(std::addressof(m_data)) T(std::forward<Args>(args_)...);
                    isSet = true;
                    cond.notify_one();
                    return std::move(m_continuation);
                }(std::forward<Args>(args)...))
            continuation->run();
    }

    T takeValue() {
        auto const lock(waitReadyLock());
        if (exception)
            std::rethrow_exception(exception);
        return std::move(*reinterpret_cast<T *>(&m_data));
    }

/* Fields: */

    typename std::aligned_storage<sizeof(T), alignof(T)>::type m_data;

};

template <>
struct SharedState<void> {

    SHAREMIND_SHAREDSTATE_COMMON

/* Methods: */

    void setReady() noexcept {
        if (auto const continuation =
                [this]() noexcept -> ContinuationPtr {
                    std::lock_guard<std::mutex> const guard(mutex);
                    assert(!isSet);
                    isSet = true;
                    cond.notify_one();
                    return std::move(m_continuation);
                }())
            continuation->run();
    }

    void takeValue() {
        auto const lock(waitReadyLock());
        if (exception)
            std::rethrow_exception(exception);
    }

};

#undef SHAREMIND_SHAREDSTATE_COMMON

template <typename F, typename Fut>
using PotentiallyWrappedReturnType =
            typename std::result_of<typename std::decay<F>::type(Fut)>::type;

template <typename T> struct Unwrap { using type = T; };
template <typename T> struct Unwrap<sharemind::Future<T> > { using type = T; };

template <typename F, typename Fut>
using UnwrappedReturnType =
        typename Unwrap<PotentiallyWrappedReturnType<F, Fut> >::type;


template <typename R>
struct ContinuationRun {
    template <typename P, typename F, typename Fut>
    static void run(P && p, F && f, Fut && fut) noexcept {
        try {
            p.setValue(f(std::forward<Fut>(fut)));
        } catch (...) {
            p.setException(std::current_exception());
        }
    }
};

template <typename T>
struct ContinuationRun<sharemind::Future<T> > {
    template <typename P, typename F, typename Fut>
    static void run(P && p, F && f, Fut && fut) noexcept {
        try {
            auto f2(f(std::forward<Fut>(fut)));
            if (f2.isValid()) {
                p.setValue(f2.takeValue());
            } else {
                p.setException(BrokenPromiseException());
            }
        } catch (...) {
            p.setException(std::current_exception());
        }
    }
};

template <>
struct ContinuationRun<sharemind::Future<void> > {
    template <typename P, typename F, typename Fut>
    static void run(P && p, F && f, Fut && fut) noexcept {
        try {
            auto f2(f(std::forward<Fut>(fut)));
            if (f2.isValid()) {
                f2.takeValue();
                p.setReady();
            } else {
                p.setException(BrokenPromiseException());
            }
        } catch (...) {
            p.setException(std::current_exception());
        }
    }
};

template <>
struct ContinuationRun<void> {
    template <typename P, typename F, typename Fut>
    static void run(P && p, F && f, Fut && fut) noexcept {
        try {
            f(std::forward<Fut>(fut));
            p.setReady();
        } catch (...) {
            p.setException(std::current_exception());
        }
    }
};

template <template <typename> class Prom,
          typename Fut,
          typename F>
struct Continuation final: ContinuationBase {

/* Types: */

    using Fun = typename std::decay<F>::type;

/* Methods: */

    template <typename ... Args>
    explicit Continuation(Fut && future, Args && ... args)
        : m_function(std::forward<Args>(args)...)
        , m_future(std::move(future))
    {}

    void run() noexcept final override {
        return ContinuationRun<PotentiallyWrappedReturnType<Fun, Fut> >::run(
                    m_promise,
                    m_function,
                    std::move(m_future));
    }

/* Fields: */

    Fun m_function;
    Prom<UnwrappedReturnType<F, Fut> > m_promise;
    Fut m_future;

};

} /* namespace Future { */
} /* namespace Detail { */

template <typename T> class Promise;


#define SHAREMIND_FUTURE_COMMON(T) \
    friend class Promise<T>; \
private: /* Types: */ \
    using SharedStatePtr = std::shared_ptr<Detail::Future::SharedState<T> >; \
public: /* Methods: */ \
    Future(Future const &) = delete; \
    Future & operator=(Future const &) = delete; \
    Future() noexcept {} \
    Future(Future &&) noexcept = default; \
    Future & operator=(Future &&) noexcept = default; \
    T takeValue() { \
        assert(m_state); \
        return SharedStatePtr(std::move(m_state))->takeValue(); \
    } \
    bool isValid() const noexcept { return m_state.operator bool(); } \
private: /* Methods: */ \
    explicit Future(SharedStatePtr && state) noexcept \
            : m_state(std::move(state)) \
    {} \
private: /* Fields: */ \
    SharedStatePtr m_state;

template <typename T>
class Future {

    static_assert(!std::is_reference<T>::value, "");

    SHAREMIND_FUTURE_COMMON(T)

public: /* Methods: */

    template <typename F>
    auto then(F && f) -> Future<Detail::Future::UnwrappedReturnType<F, Future> >
    {
        using C = Detail::Future::Continuation<Promise, Future<T>, F>;
        assert(m_state);
        auto & state = *m_state;
        std::unique_ptr<C> continuation(new C(std::move(*this),
                                              std::forward<F>(f)));
        auto r(continuation->m_promise.takeFuture());
        state.then(std::move(continuation));
        return r;
    }

};

template <>
class Future<void> {

    SHAREMIND_FUTURE_COMMON(void)

public: /* Methods: */

    template <typename F>
    auto then(F && f) -> Future<Detail::Future::UnwrappedReturnType<F, Future> >
    {
        using C = Detail::Future::Continuation<Promise, Future<void>, F>;
        assert(m_state);
        auto & state = *m_state;
        std::unique_ptr<C> continuation(new C(std::move(*this),
                                              std::forward<F>(f)));
        auto r(continuation->m_promise.takeFuture());
        state.then(std::move(continuation));
        return r;
    }

};

#undef SHAREMIND_FUTURE_COMMON


template <typename T> class PackagedTask;


#define SHAREMIND_PROMISE_COMMON(T) \
        template <typename T_> friend class PackagedTask; \
    private: /* Types: */ \
        enum Invalid_ { INVALID_ }; \
        using SharedStatePtr = \
                std::shared_ptr<Detail::Future::SharedState<T> >; \
    public: /* Methods: */ \
        Promise(Promise const &) = delete; \
        Promise & operator=(Promise const &) = delete; \
        Promise() {} \
        Promise(Promise &&) noexcept = default; \
        Promise & operator=(Promise &&) noexcept = default; \
        ~Promise() noexcept { \
            if (m_state) \
                m_state->setException( \
                        std::make_exception_ptr(BrokenPromiseException())); \
        } \
        void setException(std::exception_ptr e) noexcept { \
            assert(m_state); \
            return SharedStatePtr(std::move(m_state))->setException( \
                            std::move(e)); \
        } \
        template <typename Exception> \
        void setException(Exception && exception) noexcept { \
            return setException( \
                        std::make_exception_ptr( \
                            std::forward<Exception>(exception))); \
        } \
        bool isValid() const noexcept \
        { return static_cast<bool>(m_state); } \
        bool haveFuture() const noexcept \
        { return static_cast<bool>(m_futureState); } \
        Future<T> takeFuture() noexcept { \
            assert(m_futureState); \
            return Future<T>(std::move(m_futureState)); \
        } \
    private: /* Methods: */ \
        Promise(Invalid_ const) noexcept \
            : m_state(nullptr) \
            , m_futureState(nullptr) \
        {} \
    private: /* Fields: */ \
        SharedStatePtr m_state{ \
                std::make_shared<Detail::Future::SharedState<T> >()}; \
        SharedStatePtr m_futureState{m_state};

template <typename T>
class Promise {

    static_assert(!std::is_reference<T>::value, "");

    SHAREMIND_PROMISE_COMMON(T)

public: /* Methods: */

    template <typename ... Args>
    void emplaceValue(Args && ... args)
            noexcept(noexcept(
                std::declval<Detail::Future::SharedState<T> &>().emplaceValue(
                                    std::forward<Args>(args)...)))
    {
        assert(m_state);
        return SharedStatePtr(std::move(m_state))->emplaceValue(
                        std::forward<Args>(args)...);
    }

    void setValue(T && v)
            noexcept(noexcept(std::declval<Promise &>().emplaceValue(
                                  std::move(v))))
    { return emplaceValue(std::move(v)); }

    void setValue(T const & v)
            noexcept(noexcept(std::declval<Promise &>().emplaceValue(v)))
    { return emplaceValue(v); }

};

template <>
class Promise<void> {

    SHAREMIND_PROMISE_COMMON(void)

public: /* Methods: */

    void setValue() noexcept { return setReady(); }

    void setReady() noexcept {
        assert(m_state);
        return SharedStatePtr(std::move(m_state))->setReady();
    }

};

#undef SHAREMIND_PROMISE_COMMON


Future<void> makeReadyFuture() {
    Promise<void> p;
    p.setReady();
    return p.takeFuture();
}

template <typename T>
auto makeReadyFuture(T && value)
        -> Future<StripReferenceWrapper_t<typename std::decay<T>::type> >
{
    Promise<StripReferenceWrapper_t<typename std::decay<T>::type> > p;
    p.setValue(std::forward<T>(value));
    return p.takeFuture();
}

template <typename T>
Future<T> makeExceptionalFuture(std::exception_ptr exception) {
    Promise<T> p;
    p.setException(std::move(exception));
    return p.takeFuture();
}

template <typename T, typename Exception>
Future<T> makeExceptionalFuture(Exception && exception) {
    Promise<T> p;
    p.setException(std::move(exception));
    return p.takeFuture();
}

template <typename T>
Future<T> makeBrokenFuture() { return Promise<T>().takeFuture(); }


template <typename> class PackagedTask;

template <typename R, typename ... ArgTypes>
class PackagedTask<R(ArgTypes...)> {

private: /* Types: */

    using Function = std::function<R(ArgTypes...)>;

public: /* Methods: */

    PackagedTask()
            noexcept(std::is_nothrow_constructible<
                            Promise<R>,
                            typename Promise<R>::Invalid_
                     >::value
                     && std::is_nothrow_default_constructible<Function>::value)
        : m_promise(Promise<R>::INVALID_)
    {}

    template <typename F>
    PackagedTask(F && f)
            noexcept(std::is_nothrow_default_constructible<Promise<R> >::value
                     && std::is_nothrow_constructible<Function, F>::value)
        : m_function(std::forward<F>(f))
    {}

    PackagedTask(PackagedTask const &) = 0;
    PackagedTask & operator=(PackagedTask const &) = 0;

    PackagedTask(PackagedTask && move)
            noexcept(std::is_nothrow_move_constructible<Promise<R> >::value
                     && std::is_nothrow_move_constructible<Function>::value)
        : m_promise(std::move(move.m_promise))
        , m_function(std::move(move.m_function))
    {}

    PackagedTask & operator=(PackagedTask && move)
            noexcept(std::is_nothrow_move_assignable<Promise<R> >::value
                     && std::is_nothrow_move_assignable<Function>::value)
    {
        m_promise = std::move(move.m_promise);
        m_function = std::move(move.m_function);
    }

    template <typename ... Args>
    void operator()(Args && ... args) noexcept {
        try {
            m_promise.setValue(m_function(std::forward<Args>(args)...));
        } catch (...) {
            m_promise.setException(std::current_exception());
        }
    }

    Future<R> takeFuture() noexcept { return m_promise.takeFuture(); }

    bool isValid() const noexcept { return m_promise.isValid(); }

    void reset() noexcept { operator=(PackagedTask(std::move(m_function))); }

private: /* Methods: */

    Promise<R> m_promise;
    Function m_function;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_FUTURE_H */
