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

#ifndef SHAREMIND_CIRCBUFFERSCSP_H
#define SHAREMIND_CIRCBUFFERSCSP_H

#include <algorithm>
#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <sharemind/compiler-support/GccVersion.h>
#include <type_traits>
#include <utility>
#include "compiler-support/GccInheritConstructor.h"
#include "Durations.h"
#include "FunctionTraits.h"
#include "PotentiallyVoidTypeInfo.h"


namespace sharemind {
namespace Detail {

template <typename MutexType>
struct CircBufferScspLockingConditionVariable
{ using type = std::condition_variable_any; };

template <>
struct CircBufferScspLockingConditionVariable<std::mutex>
{ using type = std::condition_variable; };

} /* namespace Detail { */

template <typename MutexType = std::mutex>
class CircBufferScspLocking {

public: /* Constants: */

    constexpr static bool providesWait = true;

public: /* Types: */

    class ScopedReadLock: public std::unique_lock<MutexType> {

    public: /* Methods: */

        ScopedReadLock(CircBufferScspLocking & locking)
            : std::unique_lock<MutexType>(locking.m_dataAvailableMutex)
        {}

    };

public: /* Methods: */

    inline std::size_t dataAvailable() const noexcept {
        std::lock_guard<MutexType> const guard(m_dataAvailableMutex);
        return m_dataAvailable;
    }

    inline std::size_t dataAvailableNoLocking() const noexcept
    { return m_dataAvailable; }

    inline std::size_t increaseDataAvailable(std::size_t const size) noexcept {
        std::lock_guard<MutexType> const guard(m_dataAvailableMutex);
        m_dataAvailable += size;
        m_dataAvailableCondition.notify_one();
        return m_dataAvailable;
    }

    inline std::size_t decreaseDataAvailable(std::size_t const size) noexcept {
        std::lock_guard<MutexType> const guard(m_dataAvailableMutex);
        assert(m_dataAvailable >= size);
        m_dataAvailable -= size;
        m_dataAvailableCondition.notify_one();
        return m_dataAvailable;
    }

    template <typename Lock, typename LoopDuration>
    inline void wait_for(Lock & lock, LoopDuration && loopDuration) {
        m_dataAvailableCondition.wait_for(
                    lock,
                    std::forward<LoopDuration>(loopDuration));
    }

    template <typename Lock> inline void wait(Lock & lock)
    { return m_dataAvailableCondition.wait(lock); }

private: /* Fields: */

    mutable MutexType m_dataAvailableMutex;
    typename Detail::CircBufferScspLockingConditionVariable<MutexType>::type
            m_dataAvailableCondition;
    std::size_t m_dataAvailable = 0u;

};

class CircBufferNoLocking {

public: /* Constants: */

    constexpr static bool providesWait = false;

public: /* Methods: */

    inline std::size_t dataAvailable() const noexcept
    { return m_dataAvailable; }

    inline std::size_t increaseDataAvailable(std::size_t const size) noexcept {
        m_dataAvailable += size;
        return m_dataAvailable;
    }

    inline std::size_t decreaseDataAvailable(std::size_t const size) noexcept {
        m_dataAvailable -= size;
        return m_dataAvailable;
    }

private: /* Fields: */

    std::size_t m_dataAvailable = 0u;

};

namespace Detail {

template <typename T, typename Locking, bool = Locking::providesWait>
class CircBufferBase2;

/**
 * \brief A one producer, one consumer thread circular FIFO buffer.
*/
template <typename T, typename Locking_>
class CircBufferBase {

    template <typename, typename, bool>
    friend class CircBufferBase2;

private: /* Types: */

    using Locking = Locking_;

    #if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
    #warning Using workaround for unknown GCC 4.7 bug.
    #define Self CircBufferBase<T, Locking>
    #else
    using Self = CircBufferBase<T, Locking>;
    #endif

public: /* Types: */

    using ValueType = T;
    using ValueAllocType = AllocType<T>;

public: /* Methods: */

    CircBufferBase(Self const &) = delete;
    Self & operator=(Self const &) = delete;

    inline CircBufferBase(std::size_t const bufferSize = 1024u * 1024u)
        : m_buffer(new ValueAllocType[bufferSize])
        , m_bufferSize(bufferSize)
        , m_readOffset(0u)
        , m_writeOffset(0u)
    {}

    inline ~CircBufferBase() noexcept
    { delete[] static_cast<ValueAllocType *>(m_buffer); }

    /** \returns the buffer size. */
    std::size_t bufferSize() const noexcept { return m_bufferSize; }

    /** \returns a pointer to the circular FIFO storage array. */
    inline ValueType * arrayStart() const noexcept { return m_buffer; }


    /***************************************************************************
     * Procedures for producer */

    /** \returns whether the buffer is full. */
    inline bool full() const noexcept { return spaceAvailable() <= 0u; }

    /** \returns the total number of elements free. */
    inline std::size_t spaceAvailable() const noexcept {
        std::size_t const da = dataAvailable();
        assert(da <= m_bufferSize);
        return m_bufferSize - da;
    }

    /**
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements free before the buffer array
     *                                     wraps.
     * \returns the total number of elements free.
    */
    inline std::size_t spaceAvailable(std::size_t & availableUntilBufferEnd)
            const noexcept
    {
        assert(m_writeOffset < m_bufferSize);
        std::size_t const available = spaceAvailable();
        assert(available <= m_bufferSize);
        availableUntilBufferEnd = std::min(available,
                                           m_bufferSize - m_writeOffset);
        return available;
    }

    /** \returns the number of elements free before the buffer array wraps. */
    inline std::size_t spaceAvailableUntilBufferEnd() const noexcept
    { return std::min(spaceAvailable(), m_bufferSize - m_writeOffset); }

    /** \returns the current write offset in the FIFO storage array. */
    inline std::size_t writeOffset() const noexcept { return m_writeOffset; }

    /**
     * \brief Marks data as written.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the number of elements free.
    */
    inline std::size_t haveWritten(std::size_t const size) noexcept {
        std::size_t const ret = increaseDataAvailable(size);
        assert(m_writeOffset < m_bufferSize);
        std::size_t const offsetLimit = m_bufferSize - size;
        if (m_writeOffset < offsetLimit) {
            m_writeOffset += size;
        } else {
            assert(size >= (m_bufferSize - m_writeOffset));
            m_writeOffset = size - (m_bufferSize - m_writeOffset);
        }
        assert(m_writeOffset < m_bufferSize);
        return m_bufferSize - ret;
    }

    /**
     * \brief Marks data as written.
     * \param[in] size The number of elements written to the FIFO.
    */
    inline void haveWrittenNoRet(std::size_t const size) noexcept
    { (void) haveWritten(size); }

    /**
     * \brief Marks data as written.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the total number of elements free before the buffer array
     *          wraps.
    */
    inline std::size_t haveWrittenRetUbe(std::size_t const size) noexcept {
        std::size_t const ret = haveWritten(size);
        return std::min(ret, m_bufferSize - m_writeOffset);
    }

    /**
     * \brief Marks data as written.
     * \pre The list of elements written did not wrap.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the number of elements free.
    */
    inline std::size_t haveWrittenNotUntilBufferEnd(std::size_t const size)
            noexcept
    {
        std::size_t const ret = increaseDataAvailable(size);
        assert(m_writeOffset < m_bufferSize);
        assert(size < (m_bufferSize - m_writeOffset));
        m_writeOffset += size;
        assert(m_writeOffset < m_bufferSize);
        return m_bufferSize - ret;
    }

    /**
     * \brief Marks data as written.
     * \pre The list of elements written did not wrap.
     * \param[in] size The number of elements written to the FIFO.
    */
    inline void haveWrittenNotUntilBufferEndNoRet(std::size_t const size)
            noexcept
    { (void) haveWrittenNotUntilBufferEnd(size); }

    /**
     * \brief Marks data as written.
     * \pre The list of elements written did not wrap.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the total number of elements free before the buffer array
     *          wraps.
    */
    inline std::size_t haveWrittenNotUntilBufferEndRetUbe(
            std::size_t const size) noexcept
    {
        std::size_t const ret = haveWrittenNotUntilBufferEnd(size);
        return std::min(ret, m_bufferSize - m_writeOffset);
    }

    /**
     * \brief Writes the given data to the buffer.
     * \param[in] data Pointer to the input data.
     * \param[in] size Size of the input data.
     * \returns the number of elements written (might be less than size if FIFO
     *          was filled).
    */
    inline std::size_t write(T const * data, std::size_t const size) noexcept
    { return operate<WriteActions>(data, size); }


    /***************************************************************************
     * Procedures for consumer */

    /** \returns whether the buffer is empty. */
    inline bool empty() const noexcept { return dataAvailable() <= 0u; }

    /** \returns the total number of elements pending. */
    inline std::size_t dataAvailable() const noexcept
    { return m_locking.dataAvailable(); }

    /**
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements pending before the buffer
     *                                     array wraps.
     * \returns the total number of elements pending.
    */
    inline std::size_t dataAvailable(std::size_t & availableUntilBufferEnd)
            const noexcept
    {
        assert(m_readOffset < m_bufferSize);
        std::size_t const available = dataAvailable();
        assert(available <= m_bufferSize);
        availableUntilBufferEnd = std::min(available,
                                           m_bufferSize - m_readOffset);
        return available;
    }

    /** \returns the number of elements pending before the array wraps. */
    inline std::size_t dataAvailableUntilBufferEnd() const noexcept
    { return std::min(dataAvailable(), m_bufferSize - m_readOffset); }

    /** \returns the current read offset in the FIFO storage array. */
    inline std::size_t readOffset() const noexcept { return m_readOffset; }

    /**
     * \brief Marks data as consumed.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the number of elements pending.
    */
    inline std::size_t haveRead(std::size_t const size) noexcept {
        std::size_t const ret = decreaseDataAvailable(size);
        assert(m_readOffset < m_bufferSize);
        std::size_t const offsetLimit = m_bufferSize - size;
        if (m_readOffset < offsetLimit) {
            m_readOffset += size;
        } else {
            assert(size >= (m_bufferSize - m_readOffset));
            m_readOffset = size - (m_bufferSize - m_readOffset);
        }
        assert(m_readOffset < m_bufferSize);
        return ret;
    }

    /**
     * \brief Marks data as consumed.
     * \param[in] size The number of elements to drop from the FIFO.
    */
    inline void haveReadNoRet(std::size_t const size) noexcept
    { (void) haveRead(size); }

    /**
     * \brief Marks data as consumed.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the total number of elements pending before the buffer array
     *          wraps.
    */
    inline std::size_t haveReadRetUbe(std::size_t const size) noexcept {
        std::size_t const ret = haveRead(size);
        return std::min(ret, m_bufferSize - m_readOffset);
    }

    /**
     * \brief Marks data as consumed.
     * \pre The list of elements read did not wrap.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the number of elements pending.
    */
    inline std::size_t haveReadNotUntilBufferEnd(std::size_t const size)
            noexcept
    {
        std::size_t const ret = decreaseDataAvailable(size);
        assert(m_readOffset < m_bufferSize);
        assert(size < (m_bufferSize - m_readOffset));
        m_readOffset += size;
        assert(m_readOffset < m_bufferSize);
        return ret;
    }

    /**
     * \brief Marks data as consumed.
     * \pre The list of elements read did not wrap.
     * \param[in] size The number of elements to drop from the FIFO.
    */
    inline void haveReadNotUntilBufferEndNoRet(std::size_t const size) noexcept
    { (void) haveReadNotUntilBufferEnd(size); }

    /**
     * \brief Marks data as consumed.
     * \pre The list of elements read did not wrap.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the total number of elements pending before the buffer array
     *          wraps.
    */
    inline std::size_t haveReadNotUntilBufferEndRetUbe(std::size_t const size)
            noexcept
    {
        std::size_t const ret = haveReadNotUntilBufferEnd(size);
        return std::min(ret, m_bufferSize - m_readOffset);
    }

    /**
     * \brief Reads data from the buffer.
     * \param[in] data Pointer to the output data buffer.
     * \param[in] size Size of the output data buffer.
     * \returns the number of elements read (might be less than size if FIFO was
     *          emptied).
    */
    inline std::size_t read(T * const buffer, std::size_t const size) noexcept
    { return operate<ReadActions>(buffer, size); }

private: /* Methods: */

    inline std::size_t increaseDataAvailable(std::size_t const size) noexcept {
        assert(size <= m_bufferSize);
        return m_locking.increaseDataAvailable(size);
    }

    inline std::size_t decreaseDataAvailable(std::size_t const size) noexcept {
        assert(size <= m_bufferSize);
        return m_locking.decreaseDataAvailable(size);
    }

    template <typename Actions>
    inline std::size_t operate(typename Actions::OtherSideType * copyPtr,
                               std::size_t size) noexcept
    {
        assert(copyPtr);
        assert(size > 0u);
        std::size_t aUBE = Actions::availableUntilBufferEnd(this);
        if (aUBE == 0u)
            return 0u;
        std::size_t transferred = 0u;
        for (;;) {
            assert(aUBE > 0u);
            if (aUBE >= size) {
                Actions::copyAction(Actions::operatePtr(this), copyPtr, size);
                Actions::doneNoRet(this, size);
                return transferred + size;
            }
            assert(size > aUBE);
            Actions::copyAction(Actions::operatePtr(this), copyPtr, aUBE);
            std::size_t const newAUBE = Actions::doneRetUbe(this, aUBE);
            transferred += aUBE;
            if (newAUBE == 0u)
                return transferred;
            copyPtr = sharemind::ptrAdd(copyPtr, aUBE);
            size -= aUBE;
            aUBE = newAUBE;
        }
    }

    template <typename Actions, typename Actor>
    inline std::size_t operate(Actor & actor) noexcept {
        static_assert(
            noexcept(actor(std::declval<typename Actions::BufferSideType *>(),
                           static_cast<std::size_t>(42u))), "");
        using WDT = typename std::remove_pointer<
                typename sharemind::FunctionTraits<Actor>
                        ::template argument<1u>::type
            >::type;
        static_assert(std::is_const<typename Actions::BufferSideType>
                            ::value
                      == std::is_const<WDT>::value, "");

        // Is there any (contiguous) data available?
        std::size_t availableUntilBufferEnd =
                Actions::availableUntilBufferEnd(this);
        if (availableUntilBufferEnd <= 0u)
            return 0u;

        // First iteration:
        std::size_t totalTransferred;
        {
            std::size_t const toTransfer = availableUntilBufferEnd;
            totalTransferred = actor(Actions::operatePtr(this), toTransfer);
            assert(totalTransferred <= toTransfer);
            availableUntilBufferEnd =
                    Actions::doneRetUbe(this, totalTransferred);
            if ((totalTransferred < toTransfer)
                || (availableUntilBufferEnd <= 0u))
                return totalTransferred;
        }

        // Other iterations:
        std::size_t maxTransfer = std::numeric_limits<std::size_t>::max()
                                  - totalTransferred;
        for (;;) {
            assert(availableUntilBufferEnd > 0u);
            if (availableUntilBufferEnd < maxTransfer) {
                std::size_t const toTransfer = availableUntilBufferEnd;
                std::size_t const transferred =
                        actor(Actions::operatePtr(this), toTransfer);
                assert(transferred <= toTransfer);
                availableUntilBufferEnd =
                        Actions::doneRetUbe(this, transferred);
                totalTransferred += transferred;
                if ((transferred < toTransfer)
                    || (availableUntilBufferEnd <= 0u))
                    return totalTransferred;
                maxTransfer -= transferred;
            } else { // Last iteration:
                std::size_t const transferred =
                        actor(Actions::operatePtr(this), maxTransfer);
                assert(transferred <= maxTransfer);
                Actions::doneNoRet(this, transferred);
                return totalTransferred + transferred;
            }
        }
        return totalTransferred;
    }

private: /* Types: */

    static inline void copyFirstToSecondData(T const * const first,
                                             T * const second,
                                             std::size_t const size) noexcept
    { copy(first, second, size); }

    static inline void copySecondToFirstData(T * const first,
                                             T const * const second,
                                             std::size_t const size) noexcept
    { copy(second, first, size); }

    template < typename BufferSideType_
             , typename OtherSideType_
             , void (*COPY_ACTION_)(BufferSideType_ *,
                                    OtherSideType_ *,
                                    std::size_t) noexcept
             , std::size_t (Self::*OFFSET_ACTION_)() const noexcept
             , std::size_t (Self::*AVAILABLE_UBE_)() const noexcept
             , void (Self::*DONE_NO_RET_)(std::size_t) noexcept
             , std::size_t (Self::*DONE_RET_UBE_)(std::size_t) noexcept
             >
    struct FifoBufferActions {
        static_assert(std::is_const<BufferSideType_>::value
                      != std::is_const<OtherSideType_>::value, "");
        using BufferSideType = BufferSideType_;
        using OtherSideType = OtherSideType_;
        static inline void copyAction(BufferSideType_ * const a,
                                      OtherSideType_ * const b,
                                      std::size_t const size) noexcept
        { COPY_ACTION_(a, b, size); }
        static inline BufferSideType_ * operatePtr(Self const * thisPtr)
                noexcept
        {
            return const_cast<BufferSideType_ *>(
                        ptrAdd(thisPtr->arrayStart(),
                               (thisPtr->*OFFSET_ACTION_)()));
        }
        static inline std::size_t availableUntilBufferEnd(Self * thisPtr)
                noexcept
        { return (thisPtr->*AVAILABLE_UBE_)(); }
        static inline void doneNoRet(Self * thisPtr, std::size_t const size)
                noexcept
        { (thisPtr->*DONE_NO_RET_)(size); }
        static inline std::size_t doneRetUbe(Self * thisPtr,
                                             std::size_t const size) noexcept
        { return (thisPtr->*DONE_RET_UBE_)(size); }
    };

    using WriteActions =
            FifoBufferActions<typename Self::ValueType,
                              typename Self::ValueType const,
                              &Self::copySecondToFirstData,
                              &Self::writeOffset,
                              &Self::spaceAvailableUntilBufferEnd,
                              &Self::haveWrittenNoRet,
                              &Self::haveWrittenRetUbe>;

    using ReadActions =
            FifoBufferActions<typename Self::ValueType const,
                              typename Self::ValueType,
                              &Self::copyFirstToSecondData,
                              &Self::readOffset,
                              &Self::dataAvailableUntilBufferEnd,
                              &Self::haveReadNoRet,
                              &Self::haveReadRetUbe>;

public: /* Methods */

    /**
     * \brief Uses the given actor to write to the buffer.
     * \param inputProducerActor The actor producing the input.
     * \returns the number of elements written.
    */
    template <typename InputProducerActor>
    inline std::size_t write(InputProducerActor inputProducerActor)
            noexcept(noexcept(
                        std::declval<Self &>().template operate<
                                typename Self::WriteActions>(
                                    inputProducerActor)))
    { return operate<WriteActions>(inputProducerActor); }

    /**
     * \brief Uses the given actor to read from the buffer.
     * \param outputConsumerActor The actor consuming the input.
     * \returns the number of elements read.
    */
    template <typename OutputConsumerActor>
    inline std::size_t read(OutputConsumerActor outputConsumerActor)
            noexcept(noexcept(
                         std::declval<Self &>().template operate<
                                typename Self::ReadActions>(
                                    outputConsumerActor)))
    { return operate<ReadActions>(outputConsumerActor); }

    #if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
    #undef Self
    #endif

private: /* Fields :*/

    ValueType * const m_buffer;
    std::size_t const m_bufferSize;
    std::size_t m_readOffset;
    std::size_t m_writeOffset;

    mutable Locking m_locking;

}; /* template <typename T, typename Locking> class CircBufferBase { */

template <typename T, typename Locking>
class CircBufferBase2<T, Locking, false>: public CircBufferBase<T, Locking> {

public: /* Methods: */

    SHAREMIND_GCC_INHERITED_CONSTRUCTOR(CircBufferBase2,
                                        CircBufferBase,
                                        CircBufferBase<T, Locking>)

}; /* class CircBufferBase2<T, Locking, false> */

template <typename T, typename Locking>
class CircBufferBase2<T, Locking, true>: public CircBufferBase<T, Locking> {

private: /* Types: */

    struct DummyStopTest { inline void operator()() const noexcept {} };
    struct HaveDataTest {
        inline static bool test(std::size_t const in,
                                std::size_t const bufferSize) noexcept
        { (void) bufferSize; return in != 0u; }
    };
    struct HaveSpaceTest {
        inline static bool test(std::size_t const in,
                                std::size_t const bufferSize) noexcept
        { return in != bufferSize; }
    };

public: /* Methods: */

    SHAREMIND_GCC_INHERITED_CONSTRUCTOR(CircBufferBase2,
                                        CircBufferBase,
                                        CircBufferBase<T, Locking>)

    /**
     * \brief Waits until there is data pending.
     * \returns the total number of elements pending.
    */
    inline std::size_t waitSpaceAvailable() const
    { return waitSpaceAvailable_(); }

    /**
     * \brief Waits until there is data pending.
     * \param stopTest The condition for stopping.
     * \param loopDuration The interval at which to execute the stop condition.
     * \returns the total number of elements pending.
    */
    template <typename StopTest,
              typename LoopDuration =
                  sharemind::StaticLoopDuration<3u, std::chrono::microseconds> >
    inline std::size_t waitSpaceAvailable(
            StopTest && stopTest,
            LoopDuration && loopDuration = LoopDuration{}) const
    {
        return waitSpaceAvailable_(std::forward<StopTest>(stopTest),
                                   std::forward<LoopDuration>(loopDuration));
    }

    /**
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements pending before the buffer
     *                                     array wraps.
     * \returns the total number of elements free.
    */
    inline std::size_t waitSpaceAvailable(std::size_t & availableUntilBufferEnd)
            const
    { return waitSpaceAvailable_2(availableUntilBufferEnd); }

    /**
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements pending before the buffer
     *                                     array wraps.
     * \param stopTest The condition for stopping.
     * \param loopDuration The interval at which to execute the stop condition.
     * \returns the total number of elements free.
    */
    template <typename StopTest = DummyStopTest,
              typename LoopDuration =
                  sharemind::StaticLoopDuration<3u, std::chrono::microseconds> >
    inline std::size_t waitSpaceAvailable(
            std::size_t & availableUntilBufferEnd,
            StopTest && stopTest = StopTest(),
            LoopDuration && loopDuration = LoopDuration()) const
    {
        return waitSpaceAvailable_2(availableUntilBufferEnd,
                                    std::forward<StopTest>(stopTest),
                                    std::forward<LoopDuration>(loopDuration));
    }

    /** \returns the number of elements free before the buffer array wraps. */
    inline std::size_t waitSpaceAvailableUntilBufferEnd() const
    { return waitSpaceAvailableUntilBufferEnd_(); }

    /** \returns the number of elements free before the buffer array wraps. */
    template <typename StopTest = DummyStopTest,
              typename LoopDuration =
                  sharemind::StaticLoopDuration<3u, std::chrono::microseconds> >
    inline std::size_t waitSpaceAvailableUntilBufferEnd(
            StopTest && stopTest = StopTest(),
            LoopDuration && loopDuration = LoopDuration()) const
    {
        return waitSpaceAvailableUntilBufferEnd_(
                    std::forward<StopTest>(stopTest),
                    std::forward<LoopDuration>(loopDuration));
    }

    /**
     * \brief Waits until there is data pending.
     * \returns the total number of elements pending.
    */
    inline std::size_t waitDataAvailable() const
    { return waitAvailable<HaveDataTest>(); }

    /**
     * \brief Waits until there is data pending.
     * \param stopTest The condition for stopping.
     * \param loopDuration The interval at which to execute the stop condition.
     * \returns the total number of elements pending.
    */
    template <typename StopTest,
              typename LoopDuration =
                  sharemind::StaticLoopDuration<3u, std::chrono::microseconds> >
    inline std::size_t waitDataAvailable(
            StopTest && stopTest,
            LoopDuration && loopDuration = LoopDuration{}) const
    {
        return waitAvailable<HaveDataTest>(
                    std::forward<StopTest>(stopTest),
                    std::forward<LoopDuration>(loopDuration));
    }

    /**
     * \brief Waits until there is data pending.
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements pending before the buffer
     *                                     array wraps.
     * \returns the total number of elements pending.
    */
    inline std::size_t waitDataAvailable(std::size_t & availableUntilBufferEnd)
            const
    {
        return waitDataAvailable_(availableUntilBufferEnd);
        std::size_t const toBufferEnd = (this->bufferSize() - this->readOffset());
        std::size_t const available =
                waitAvailable<HaveDataTest>();
        availableUntilBufferEnd = std::min(available, toBufferEnd);
        return available;
    }

    /**
     * \brief Waits until there is data pending.
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements pending before the buffer
     *                                     array wraps.
     * \param stopTest The condition for stopping.
     * \param loopDuration The interval at which to execute the stop condition.
     * \returns the total number of elements pending.
    */
    template <typename StopTest,
              typename LoopDuration =
                  sharemind::StaticLoopDuration<3u, std::chrono::microseconds> >
    inline std::size_t waitDataAvailable(
            std::size_t & availableUntilBufferEnd,
            StopTest && stopTest,
            LoopDuration && loopDuration = LoopDuration{}) const
    {
        return waitDataAvailable_(availableUntilBufferEnd,
                                  std::forward<StopTest>(stopTest),
                                  std::forward<LoopDuration>(loopDuration));
    }

    /**
     * \brief Waits until there is data pending.
     * \returns the number of elements pending before the buffer array wraps.
    */
    inline std::size_t waitDataAvailableUntilBufferEnd() const
    { return waitDataAvailableUntilBufferEnd_(); }

    /**
     * \brief Waits until there is data pending.
     * \param stopTest The condition for stopping.
     * \param loopDuration The interval at which to execute the stop condition.
     * \returns the number of elements pending before the buffer array wraps.
    */
    template <typename StopTest,
              typename LoopDuration =
                  sharemind::StaticLoopDuration<3u, std::chrono::microseconds> >
    inline std::size_t waitDataAvailableUntilBufferEnd(
            StopTest && stopTest,
            LoopDuration && loopDuration = LoopDuration{}) const
    {
        return waitDataAvailableUntilBufferEnd_(
                    std::forward<StopTest>(stopTest),
                    std::forward<LoopDuration>(loopDuration));
    }

private: /* Methods: */

    template <typename ... Args>
    inline std::size_t waitSpaceAvailable_(Args && ... args) const {
        return this->bufferSize() - waitAvailable<HaveSpaceTest>(
                    std::forward<Args>(args)...);
    }

    template <typename ... Args>
    inline std::size_t waitSpaceAvailable_2(
            std::size_t & availableUntilBufferEnd,
            Args && ... args) const
    {
        std::size_t const available =
                waitAvailable<HaveSpaceTest>(std::forward<Args>(args)...);
        availableUntilBufferEnd = this->bufferSize()
                                  - std::max(available, this->writeOffset());
        return this->bufferSize() - available;
    }

    template <typename ... Args>
    inline std::size_t waitSpaceAvailableUntilBufferEnd_(Args && ... args) const
    {
        return this->bufferSize()
               - std::max(waitAvailable<HaveSpaceTest>(
                              std::forward<Args>(args)...),
                          this->writeOffset());
    }

    template <typename ... Args>
    inline std::size_t waitDataAvailable_(
            std::size_t & availableUntilBufferEnd,
            Args && ... args) const
    {
        std::size_t const toBufferEnd = this->bufferSize() - this->readOffset();
        std::size_t const available =
                waitAvailable<HaveDataTest>(std::forward<Args>(args)...);
        availableUntilBufferEnd = std::min(available, toBufferEnd);
        return available;
    }

    template <typename ... Args>
    inline std::size_t waitDataAvailableUntilBufferEnd_(Args && ... args) const
    {
        std::size_t const toBufferEnd = (this->bufferSize() - this->readOffset());
        return std::min(waitAvailable<HaveDataTest>(
                            std::forward<Args>(args)...),
                        toBufferEnd);
    }

    template <typename Condition>
    inline std::size_t waitAvailable() const {
        Locking & lockingImpl = this->m_locking;
        typename Locking::ScopedReadLock lock(lockingImpl);
        while (!Condition::test(lockingImpl.dataAvailableNoLocking(),
                                this->bufferSize()))
            lockingImpl.wait(lock);
        return lockingImpl.dataAvailableNoLocking();
    }

    template <typename Condition, typename StopTest, typename LoopDuration>
    inline std::size_t waitAvailable(StopTest stopTest,
                                     LoopDuration const loopDuration) const
    {
        Locking & lockingImpl = this->m_locking;
        typename Locking::ScopedReadLock lock(lockingImpl);
        while (!Condition::test(lockingImpl.dataAvailableNoLocking(),
                                this->bufferSize()))
        {
            stopTest();
            lockingImpl.wait_for(lock, loopDuration);
        }
        return lockingImpl.dataAvailableNoLocking();
    }

}; /* class CircBufferBase2<T, Locking, true> */

} /* namespace Detail { */

template <typename T, typename Locking = CircBufferScspLocking<> >
using CircBufferSCSP = Detail::CircBufferBase2<T, Locking>;

} /* namespace sharemind { */

#endif /* SHAREMIND_CIRCBUFFERSCSP_H */
