#include "../src/CircBufferSCSP.h"

#include <atomic>
#include <cstddef>
#include <thread>
#include <utility>
#include "../src/TestAssert.h"


struct B : sharemind::CircBufferSCSP<char> {
    B(std::size_t fullSize, std::size_t filledSize)
        : sharemind::CircBufferSCSP<char>(fullSize)
    {
        SHAREMIND_TESTASSERT(filledSize <= fullSize);
        ValueType elem = 0;
        while (filledSize--) {
            write(&elem, 1u);
            ++elem;
        }
    }
};

template <std::size_t SIZE,
          std::size_t FILL_A,
          std::size_t FILL_B,
          std::size_t ITERS,
          typename Worker>
void testWithActors(Worker worker) {
    static_assert(FILL_A <= SIZE, "");
    static_assert(FILL_B <= SIZE, "");
    static_assert((FILL_A < SIZE) || (FILL_B < SIZE), "");
    static_assert((FILL_A > 0u) || (FILL_B > 0u), "");
    B aInit(SIZE, FILL_A);
    B bInit(SIZE, FILL_B);
    B a(SIZE, FILL_A);
    B b(SIZE, FILL_B);

    std::atomic<bool> startSignal(false);
    std::thread t1(worker, std::ref(startSignal), std::ref(a), std::ref(b));
    std::thread t2(worker, std::ref(startSignal), std::ref(b), std::ref(a));
    startSignal.store(true, std::memory_order_release);
    t1.join();
    t2.join();
    SHAREMIND_TESTASSERT(a.dataAvailable() == aInit.dataAvailable());
    SHAREMIND_TESTASSERT(b.dataAvailable() == bInit.dataAvailable());

    static auto const comp =
            [](B & a_, B & b_) noexcept {
                auto s = a_.dataAvailable();
                SHAREMIND_TESTASSERT(s == b_.dataAvailable());
                for (; s; --s) {
                    B::ValueType v1{};
                    B::ValueType v2{};
                    a_.read(&v1, 1u);
                    b_.read(&v2, 1u);
                    SHAREMIND_TESTASSERT(v1 == v2);
                }
            };
    comp(a, aInit);
    comp(b, bInit);
}

#define WORKER(rw,tf,wr,ft,c,...) \
        [](std::atomic<bool> & startSignal, B & from, B & to) noexcept { \
            auto toMove = (FILL_A + FILL_B) * ITERS; \
            while (!startSignal.load(std::memory_order_acquire)) {}; \
            do { \
                for (;;) { \
                    auto rw ## Actor = \
                            [toMove, &tf](char c * const ptr, \
                                          std::size_t size) \
                                    mutable noexcept -> std::size_t \
                            { \
                                __VA_ARGS__ \
                                std::size_t const s = \
                                        (size > toMove) ? toMove : size; \
                                if (s <= 0u) \
                                    return 0u; \
                                auto r = tf.rw(ptr, s); \
                                SHAREMIND_TESTASSERT(r <= s); \
                                SHAREMIND_TESTASSERT(r <= toMove); \
                                toMove -= r; \
                                return r; \
                            }; \
                    auto const moved = ft.wr(rw ## Actor); \
                    SHAREMIND_TESTASSERT(moved <= toMove); \
                    if (moved > 0u) { \
                        toMove -= moved; \
                        break; \
                    } \
                    from.waitDataAvailable(); \
                    to.waitSpaceAvailable(); \
                } \
            } while (toMove); \
        }

template <std::size_t SIZE,
          std::size_t FILL_A,
          std::size_t FILL_B,
          std::size_t ITERS>
inline void testNoStep() {
    testWithActors<SIZE, FILL_A, FILL_B, ITERS>(
                WORKER(read,  from, write, to,,));
    testWithActors<SIZE, FILL_A, FILL_B, ITERS>(
                WORKER(write, to,   read,  from, const,));
}

template <std::size_t SIZE,
          std::size_t FILL_A,
          std::size_t FILL_B,
          std::size_t ITERS,
          std::size_t STEP>
inline void test() {
    testNoStep<SIZE, FILL_A, FILL_B, ITERS>();
    testWithActors<SIZE, FILL_A, FILL_B, ITERS>(
                WORKER(read,  from, write, to,,
                       if (size > STEP) size = STEP;));
    testWithActors<SIZE, FILL_A, FILL_B, ITERS>(
                WORKER(write, to,   read,  from, const,
                       if (size > STEP) size = STEP;));
}

int main() {
    testNoStep<1u, 0u, 1u, 1000u>();
    test<10u, 0u, 5u, 1000u, 3u>();
    test<10u, 1u, 5u, 1000u, 3u>();
    test<10u, 2u, 5u, 1000u, 3u>();
    test<10u, 3u, 5u, 1000u, 3u>();
    test<10u, 4u, 5u, 1000u, 3u>();
    test<10u, 5u, 5u, 1000u, 3u>();
    test<10u, 6u, 5u, 1000u, 3u>();
    test<10u, 7u, 5u, 1000u, 3u>();
    test<10u, 8u, 5u, 1000u, 3u>();
    test<10u, 9u, 5u, 1000u, 3u>();
    test<10u, 10u, 5u, 1000u, 3u>();
    test<10u, 10u, 5u, 1000u, 7u>();
    test<10u, 0u, 10u, 1000u, 3u>();
    test<10u, 1u, 10u, 1000u, 3u>();
    test<10u, 2u, 10u, 1000u, 3u>();
    test<10u, 3u, 10u, 1000u, 3u>();
    test<10u, 4u, 10u, 1000u, 3u>();
    test<10u, 5u, 10u, 1000u, 3u>();
    test<10u, 6u, 10u, 1000u, 3u>();
    test<10u, 7u, 10u, 1000u, 3u>();
    test<10u, 8u, 10u, 1000u, 3u>();
    test<10u, 9u, 10u, 1000u, 3u>();
    test<100u, 0u, 1u, 1000u, 3u>();
    test<100u, 0u, 10u, 1000u, 3u>();
    test<100u, 5u, 10u, 1000u, 3u>();
    test<100u, 13u, 50u, 1000u, 17u>();
    test<100u, 50u, 50u, 1000u, 29u>();
    test<100u, 77u, 50u, 1000u, 29u>();
    test<100u, 100u, 50u, 1000u, 3u>();
}
