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
#include <exception>
#include <sharemind/comma.h>
#include <type_traits>
#include <utility>
#include "compiler-support/GccInheritConstructor.h"
#include "compiler-support/GccPR50025.h"
#include "compiler-support/GccVersion.h"
#include "CountMaxActor.h"
#include "Durations.h"
#include "Exception.h"
#include "FunctionTraits.h"
#include "PartialStreamOperationException.h"
#include "PotentiallyVoidTypeInfo.h"
#include "QueueingRwMutex.h"


namespace sharemind {

class CircBufferDefaultLocking {

public: /* Constants: */

    constexpr static bool const providesWait = true;

private: /* Types: */

    using WriteGuard = sharemind::QueueingRwMutex::UniqueGuard;
    using ReadGuard = sharemind::QueueingRwMutex::SharedGuard;

public: /* Types: */

    class ScopedReadLock: public sharemind::QueueingRwMutex::SharedLock {

    public: /* Methods: */

        ScopedReadLock(CircBufferDefaultLocking & locking)
            : sharemind::QueueingRwMutex::SharedLock(
                  locking.m_dataAvailableMutex)
        {}

    };

public: /* Methods: */

    inline size_t dataAvailable() const noexcept {
        ReadGuard const guard(m_dataAvailableMutex);
        return m_dataAvailable;
    }

    inline size_t dataAvailableNoLocking() const noexcept
    { return m_dataAvailable; }

    inline size_t increaseDataAvailable(size_t const size) noexcept {
        WriteGuard const guard(m_dataAvailableMutex);
        m_dataAvailable += size;
        m_dataAvailableCondition.notify_one();
        return m_dataAvailable;
    }

    inline size_t decreaseDataAvailable(size_t const size) noexcept {
        WriteGuard const guard(m_dataAvailableMutex);
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

    mutable sharemind::QueueingRwMutex m_dataAvailableMutex;
    std::condition_variable_any m_dataAvailableCondition;
    size_t m_dataAvailable = 0u;

};

class CircBufferNoLocking {

public: /* Constants: */

    constexpr static bool const providesWait = false;

public: /* Methods: */

    inline size_t dataAvailable() const noexcept { return m_dataAvailable; }

    inline size_t increaseDataAvailable(size_t const size) noexcept {
        m_dataAvailable += size;
        return m_dataAvailable;
    }

    inline size_t decreaseDataAvailable(size_t const size) noexcept {
        m_dataAvailable -= size;
        return m_dataAvailable;
    }

private: /* Fields: */

    size_t m_dataAvailable = 0u;

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
    typedef CircBufferBase<T, Locking> Self;
    #endif

public: /* Types: */

    typedef T ValueType;
    typedef sharemind::PotentiallyVoidTypeInfo<T> ValueTypeInfo;
    typedef typename ValueTypeInfo::allocType ValueAllocType;

    class ReadActor {

    public: /* Types: */

        SHAREMIND_DEFINE_EXCEPTION_UNUSED(std::exception, Exception);

    private: /* Types: */

        using WantDataType = T;

    public: /* Methods: */

        ReadActor(ReadActor &&) = default;
        ReadActor(const ReadActor &) = default;
        ReadActor & operator=(ReadActor &&) = default;
        ReadActor & operator=(const ReadActor &) = default;

        inline ReadActor(Self & buffer) noexcept
            : m_buffer SHAREMIND_GCCPR50025_WORKAROUND(buffer)
        {}

        inline size_t operator()(T * const data, const size_t size) noexcept
        { return m_buffer.read(data, size); }

    private: /* Fields: */

        Self & m_buffer;

    };

    class WriteActor {

    public: /* Types: */

        SHAREMIND_DEFINE_EXCEPTION_UNUSED(std::exception, Exception);

    private: /* Types: */

        using WantDataType = const T;

    public: /* Methods: */

        WriteActor(WriteActor &&) = default;
        WriteActor(const WriteActor &) = default;
        WriteActor & operator=(WriteActor &&) = default;
        WriteActor & operator=(const WriteActor &) = default;

        inline WriteActor(Self & buffer) noexcept
            : m_buffer SHAREMIND_GCCPR50025_WORKAROUND(buffer)
        {}

        inline size_t operator()(const T * data, const size_t size) noexcept
        { return m_buffer.write(data, size); }

    private: /* Fields: */

        Self & m_buffer;

    };

public: /* Methods: */

    CircBufferBase(const Self &) = delete;
    Self & operator=(const Self &) = delete;

    inline CircBufferBase(const size_t bufferSize = 1024u * 1024u)
        : m_buffer(new ValueAllocType[bufferSize])
        , m_bufferSize(bufferSize)
        , m_readOffset(0u)
        , m_writeOffset(0u)
    {}

    inline ~CircBufferBase() noexcept
    { delete[] static_cast<ValueAllocType *>(m_buffer); }

    /** \returns the buffer size. */
    size_t bufferSize() const noexcept { return m_bufferSize; }

    /** \returns a pointer to the circular FIFO storage array. */
    inline ValueType * arrayStart() const noexcept { return m_buffer; }


    /***************************************************************************
     * Procedures for producer */

    /** \returns whether the buffer is full. */
    inline bool full() const noexcept { return spaceAvailable() <= 0u; }

    /** \returns the total number of elements free. */
    inline size_t spaceAvailable() const noexcept {
        const size_t da = dataAvailable();
        assert(da <= m_bufferSize);
        return m_bufferSize - da;
    }

    /**
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements free before the buffer array
     *                                     wraps.
     * \returns the total number of elements free.
    */
    inline size_t spaceAvailable(size_t & availableUntilBufferEnd)
            const noexcept
    {
        assert(m_writeOffset < m_bufferSize);
        const size_t available = spaceAvailable();
        assert(available <= m_bufferSize);
        availableUntilBufferEnd = std::min(available,
                                           m_bufferSize - m_writeOffset);
        return available;
    }

    /** \returns the number of elements free before the buffer array wraps. */
    inline size_t spaceAvailableUntilBufferEnd() const noexcept
    { return std::min(spaceAvailable(), m_bufferSize - m_writeOffset); }

    /** \returns the current write offset in the FIFO storage array. */
    inline size_t writeOffset() const noexcept { return m_writeOffset; }

    /**
     * \brief Marks data as written.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the number of elements free.
    */
    inline size_t haveWritten(const size_t size) noexcept {
        const size_t ret = increaseDataAvailable(size);
        assert(m_writeOffset < m_bufferSize);
        const size_t offsetLimit = m_bufferSize - size;
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
    inline void haveWrittenNoRet(const size_t size) noexcept
    { (void) haveWritten(size); }

    /**
     * \brief Marks data as written.
     * \param[in] size The number of elements written to the FIFO.
     * \returns a pair where the first element is the number of items free, and
     *          the second element is the total number of elements free before
     *          the buffer array wraps.
    */
    inline std::pair<size_t, size_t> haveWrittenRetPair(const size_t size)
            noexcept
    {
        const size_t ret = haveWritten(size);
        return std::make_pair(ret, std::min(ret, m_bufferSize - m_writeOffset));
    }

    /**
     * \brief Marks data as written.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the total number of elements free before the buffer array
     *          wraps.
    */
    inline size_t haveWrittenRetUbe(const size_t size) noexcept {
        const size_t ret = haveWritten(size);
        return std::min(ret, m_bufferSize - m_writeOffset);
    }

    /**
     * \brief Marks data as written.
     * \pre The list of elements written did not wrap.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the number of elements free.
    */
    inline size_t haveWrittenNotUntilBufferEnd(const size_t size) noexcept {
        const size_t ret = increaseDataAvailable(size);
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
    inline void haveWrittenNotUntilBufferEndNoRet(const size_t size) noexcept
    { (void) haveWrittenNotUntilBufferEnd(size); }

    /**
     * \brief Marks data as written.
     * \pre The list of elements written did not wrap.
     * \param[in] size The number of elements written to the FIFO.
     * \returns a pair where the first element is the number of items free, and
     *          the second element is the total number of elements free before
     *          the buffer array wraps.
    */
    inline std::pair<size_t, size_t> haveWrittenNotUntilBufferEndRetPair(
            const size_t size) noexcept
    {
        const size_t ret = haveWrittenNotUntilBufferEnd(size);
        return std::make_pair(ret, std::min(ret, m_bufferSize - m_writeOffset));
    }

    /**
     * \brief Marks data as written.
     * \pre The list of elements written did not wrap.
     * \param[in] size The number of elements written to the FIFO.
     * \returns the total number of elements free before the buffer array
     *          wraps.
    */
    inline size_t haveWrittenNotUntilBufferEndRetUbe(const size_t size) noexcept
    {
        const size_t ret = haveWrittenNotUntilBufferEnd(size);
        return std::min(ret, m_bufferSize - m_writeOffset);
    }

    /**
     * \brief Writes the given data to the buffer.
     * \param[in] data Pointer to the input data.
     * \param[in] size Size of the input data.
     * \returns the number of elements written (might be less than size if FIFO
     *          was filled).
    */
    inline size_t write(const T * data, const size_t size) noexcept
    { return operate<WriteActions>(data, size); }


    /***************************************************************************
     * Procedures for consumer */

    /** \returns whether the buffer is empty. */
    inline bool empty() const noexcept { return dataAvailable() <= 0u; }

    /** \returns the total number of elements pending. */
    inline size_t dataAvailable() const noexcept
    { return m_locking.dataAvailable(); }

    /**
     * \param[out] availableUntilBufferEnd where to write the total number of
     *                                     elements pending before the buffer
     *                                     array wraps.
     * \returns the total number of elements pending.
    */
    inline size_t dataAvailable(size_t & availableUntilBufferEnd) const noexcept
    {
        assert(m_readOffset < m_bufferSize);
        const size_t available = dataAvailable();
        assert(available <= m_bufferSize);
        availableUntilBufferEnd = std::min(available,
                                           m_bufferSize - m_readOffset);
        return available;
    }

    /** \returns the number of elements pending before the array wraps. */
    inline size_t dataAvailableUntilBufferEnd() const noexcept
    { return std::min(dataAvailable(), m_bufferSize - m_readOffset); }

    /** \returns the current read offset in the FIFO storage array. */
    inline size_t readOffset() const noexcept { return m_readOffset; }

    /**
     * \brief Marks data as consumed.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the number of elements pending.
    */
    inline size_t haveRead(const size_t size) noexcept {
        const size_t ret = decreaseDataAvailable(size);
        assert(m_readOffset < m_bufferSize);
        const size_t offsetLimit = m_bufferSize - size;
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
    inline void haveReadNoRet(const size_t size) noexcept
    { (void) haveRead(size); }

    /**
     * \brief Marks data as consumed.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns a pair where the first element is the number of items pending,
     *          and the second element is the total number of elements pending
     *          before the buffer array wraps.
    */
    inline std::pair<size_t, size_t> haveReadRetPair(const size_t size) noexcept
    {
        const size_t ret = haveRead(size);
        return std::make_pair(ret, std::min(ret, m_bufferSize - m_readOffset));
    }

    /**
     * \brief Marks data as consumed.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the total number of elements pending before the buffer array
     *          wraps.
    */
    inline size_t haveReadRetUbe(const size_t size) noexcept {
        const size_t ret = haveRead(size);
        return std::min(ret, m_bufferSize - m_readOffset);
    }

    /**
     * \brief Marks data as consumed.
     * \pre The list of elements read did not wrap.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the number of elements pending.
    */
    inline size_t haveReadNotUntilBufferEnd(const size_t size) noexcept {
        const size_t ret = decreaseDataAvailable(size);
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
    inline void haveReadNotUntilBufferEndNoRet(const size_t size) noexcept {
        (void) haveReadNotUntilBufferEnd(size);
    }

    /**
     * \brief Marks data as consumed.
     * \pre The list of elements read did not wrap.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns a pair where the first element is the number of items pending,
     *          and the second element is the total number of elements pending
     *          before the buffer array wraps.
    */
    inline std::pair<size_t, size_t> haveReadNotUntilBufferEndRetPair(
            const size_t size) noexcept
    {
        const size_t ret = haveReadNotUntilBufferEnd(size);
        return std::make_pair(ret, std::min(ret, m_bufferSize - m_readOffset));
    }

    /**
     * \brief Marks data as consumed.
     * \pre The list of elements read did not wrap.
     * \param[in] size The number of elements to drop from the FIFO.
     * \returns the total number of elements pending before the buffer array
     *          wraps.
    */
    inline size_t haveReadNotUntilBufferEndRetUbe(const size_t size) noexcept {
        const size_t ret = haveReadNotUntilBufferEnd(size);
        return std::min(ret, m_bufferSize - m_readOffset);
    }

    /**
     * \brief Reads data from the buffer.
     * \param[in] data Pointer to the output data buffer.
     * \param[in] size Size of the output data buffer.
     * \returns the number of elements read (might be less than size if FIFO was
     *          emptied).
    */
    inline size_t read(T * const buffer, const size_t size) noexcept
    { return operate<ReadActions>(buffer, size); }

private: /* Methods: */

    inline size_t increaseDataAvailable(const size_t size) noexcept {
        assert(size <= m_bufferSize);
        return m_locking.increaseDataAvailable(size);
    }

    inline size_t decreaseDataAvailable(const size_t size) noexcept {
        assert(size <= m_bufferSize);
        return m_locking.decreaseDataAvailable(size);
    }

    template <typename Actions>
    inline size_t operate(typename Actions::OtherSideType * readPtr,
                          size_t size) noexcept
    {
        assert(readPtr);
        assert(size > 0u);
        size_t aUBE = Actions::availableUntilBufferEnd(this);
        if (aUBE == 0u)
            return 0u;
        size_t transferred = 0u;
        for (;;) {
            assert(aUBE > 0u);
            if (aUBE >= size) {
                Actions::copyAction(Actions::operatePtr(this), readPtr, size);
                Actions::doneNoRet(this, size);
                return transferred + size;
            }
            assert(size > aUBE);
            Actions::copyAction(Actions::operatePtr(this), readPtr, aUBE);
            const size_t newAUBE = Actions::doneRetUbe(this, aUBE);
            transferred += aUBE;
            if (newAUBE == 0u)
                return transferred;
            readPtr = sharemind::ptrAdd(readPtr, aUBE);
            size -= aUBE;
            aUBE = newAUBE;
        }
    }

    template <typename Actions, typename Actor>
    inline size_t operate(Actor && actor)
            noexcept(noexcept(
                         actor(
                             std::declval<typename Actions::BufferSideType *>(),
                             static_cast<size_t>(42u))))
    {
        CountMaxActor<Actor> countActor(std::forward<Actor>(actor));
        using WDT = typename std::remove_pointer<
                typename sharemind::FunctionTraits<Actor>
                        ::template argument<1u>::type
            >::type;
        static_assert(std::is_const<typename Actions::BufferSideType>
                            ::value
                      == std::is_const<WDT>::value, "");
        size_t availableUntilBufferEnd = Actions::availableUntilBufferEnd(this);
        while (availableUntilBufferEnd > 0u) {
            try {
                const size_t toTransfer = availableUntilBufferEnd;
                const size_t transferred = countActor(Actions::operatePtr(this),
                                                      toTransfer);
                assert(transferred <= toTransfer);
                availableUntilBufferEnd = Actions::doneRetUbe(this,
                                                              transferred);
                if (transferred < toTransfer)
                    return countActor.count();
            } catch (typename CountMaxActor<Actor>::Exception const &) {
                PartialStreamOperationException::throwWithCurrent(
                            countActor.count());
            } catch (PartialStreamOperationException & e) {
                e.addToSize(countActor.count());
                throw;
            } catch (...) {
                std::unexpected();
            }
        }
        return countActor.count();
    }

private: /* Types: */

    static inline void copyFirstToSecondData(const T * const first,
                                             T * const second,
                                             const size_t size) noexcept
    { ValueTypeInfo::copy(first, second, size); }

    static inline void copySecondToFirstData(T * const first,
                                             const T * const second,
                                             const size_t size) noexcept
    { ValueTypeInfo::copy(second, first, size); }

    template < typename BufferSideType_
             , typename OtherSideType_
             , void (*COPY_ACTION_)(BufferSideType_ *,
                                     OtherSideType_ *,
                                     size_t) noexcept
             , size_t (Self::*OFFSET_ACTION_)() const noexcept
             , size_t (Self::*AVAILABLE_WITH_REF_ACTION_)(size_t &)
                    const noexcept
             , size_t (Self::*AVAILABLE_UBE_)() const noexcept
             , size_t (Self::*DONE_)(size_t) noexcept
             , void (Self::*DONE_NO_RET_)(size_t) noexcept
             , std::pair<size_t, size_t> (Self::*DONE_RET_PAIR_)(size_t)
                    noexcept
             , size_t (Self::*DONE_RET_UBE_)(size_t) noexcept
             >
    struct FifoBufferActions {
        static_assert(std::is_const<BufferSideType_>::value
                      != std::is_const<OtherSideType_>::value, "");
        typedef BufferSideType_ BufferSideType;
        typedef OtherSideType_ OtherSideType;
        static inline void copyAction(BufferSideType_ * const a,
                                      OtherSideType_ * const b,
                                      const size_t size) noexcept
        { COPY_ACTION_(a, b, size); }
        static inline size_t offset(const Self * thisPtr) noexcept
        { return (thisPtr->*OFFSET_ACTION_)(); }
        static inline BufferSideType_ * operatePtr(const Self * thisPtr)
                noexcept
        {
            return const_cast<BufferSideType_ *>(
                        ValueTypeInfo::ptrAdd(thisPtr->arrayStart(),
                                              offset(thisPtr)));
        }
        static inline size_t available(Self * thisPtr,
                                       size_t & availableUntilBufferEnd)
                noexcept
        {
            return (thisPtr->*AVAILABLE_WITH_REF_ACTION_)(
                        availableUntilBufferEnd);
        }
        static inline size_t availableUntilBufferEnd(Self * thisPtr) noexcept
        { return (thisPtr->*AVAILABLE_UBE_)(); }
        static inline size_t done(Self * thisPtr, const size_t size) noexcept
        { return (thisPtr->*DONE_)(size); }
        static inline void doneNoRet(Self * thisPtr, const size_t size) noexcept
        { (thisPtr->*DONE_NO_RET_)(size); }
        static inline std::pair<size_t, size_t> doneRetPair(
                Self * thisPtr,
                const size_t size) noexcept
        { return (thisPtr->*DONE_RET_PAIR_)(size); }
        static inline size_t doneRetUbe(Self * thisPtr,
                                        const size_t size) noexcept
        { return (thisPtr->*DONE_RET_UBE_)(size); }
    };

    typedef FifoBufferActions<typename Self::ValueType,
                              const typename Self::ValueType,
                              &Self::copySecondToFirstData,
                              &Self::writeOffset,
                              &Self::spaceAvailable,
                              &Self::spaceAvailableUntilBufferEnd,
                              &Self::haveWritten,
                              &Self::haveWrittenNoRet,
                              &Self::haveWrittenRetPair,
                              &Self::haveWrittenRetUbe> WriteActions;

    typedef FifoBufferActions<const typename Self::ValueType,
                              typename Self::ValueType,
                              &Self::copyFirstToSecondData,
                              &Self::readOffset,
                              &Self::dataAvailable,
                              &Self::dataAvailableUntilBufferEnd,
                              &Self::haveRead,
                              &Self::haveReadNoRet,
                              &Self::haveReadRetPair,
                              &Self::haveReadRetUbe> ReadActions;

public: /* Methods */

    /**
     * \brief Uses the given actor to write to the buffer.
     * \param inputProducerActor The actor producing the input.
     * \returns the number of elements written.
    */
    template <typename InputProducerActor>
    inline size_t write(InputProducerActor && inputProducerActor)
            noexcept(noexcept(
                        std::declval<Self &>().template operate<
                                typename Self::WriteActions,
                                InputProducerActor>(
                             std::forward<InputProducerActor>(
                                 inputProducerActor))))
    {
        return operate<WriteActions, InputProducerActor>(
                    std::forward<InputProducerActor>(inputProducerActor));
    }

    /**
     * \brief Uses the given actor to read from the buffer.
     * \param outputConsumerActor The actor consuming the input.
     * \returns the number of elements read.
    */
    template <typename OutputConsumerActor>
    inline size_t read(OutputConsumerActor && outputConsumerActor)
            noexcept(noexcept(
                         std::declval<Self &>().template operate<
                                Self::ReadActions,
                                OutputConsumerActor>(
                             std::forward<OutputConsumerActor>(
                                 outputConsumerActor))))
    {
        return operate<ReadActions, OutputConsumerActor>(
                    std::forward<OutputConsumerActor>(outputConsumerActor));
    }

    #if defined(SHAREMIND_GCC_VERSION) && (SHAREMIND_GCC_VERSION < 40800)
    #undef Self
    #endif

private: /* Fields :*/

    ValueType * const m_buffer;
    const size_t m_bufferSize;
    size_t m_readOffset;
    size_t m_writeOffset;

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
        inline static bool test(const size_t in, const size_t bufferSize)
                noexcept
        { (void) bufferSize; return in != 0u; }
    };
    struct HaveSpaceTest {
        inline static bool test(const size_t in, const size_t bufferSize)
                noexcept
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
    inline size_t waitSpaceAvailable() const
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
    inline size_t waitSpaceAvailable(
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
    inline size_t waitSpaceAvailable(size_t & availableUntilBufferEnd) const
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
    inline size_t waitSpaceAvailable(
            size_t & availableUntilBufferEnd,
            StopTest && stopTest = StopTest(),
            LoopDuration && loopDuration = LoopDuration()) const
    {
        return waitSpaceAvailable_2(availableUntilBufferEnd,
                                    std::forward<StopTest>(stopTest),
                                    std::forward<LoopDuration>(loopDuration));
    }

    /** \returns the number of elements free before the buffer array wraps. */
    inline size_t waitSpaceAvailableUntilBufferEnd() const {
        return waitSpaceAvailableUntilBufferEnd_();
    }

    /** \returns the number of elements free before the buffer array wraps. */
    template <typename StopTest = DummyStopTest,
              typename LoopDuration =
                  sharemind::StaticLoopDuration<3u, std::chrono::microseconds> >
    inline size_t waitSpaceAvailableUntilBufferEnd(
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
    inline size_t waitDataAvailable() const
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
    inline size_t waitDataAvailable(
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
    inline size_t waitDataAvailable(size_t & availableUntilBufferEnd) const {
        return waitDataAvailable_(availableUntilBufferEnd);
        const size_t toBufferEnd = (this->bufferSize() - this->readOffset());
        const size_t available =
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
    inline size_t waitDataAvailable(
            size_t & availableUntilBufferEnd,
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
    inline size_t waitDataAvailableUntilBufferEnd() const
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
    inline size_t waitDataAvailableUntilBufferEnd(
            StopTest && stopTest,
            LoopDuration && loopDuration = LoopDuration{}) const
    {
        return waitDataAvailableUntilBufferEnd_(
                    std::forward<StopTest>(stopTest),
                    std::forward<LoopDuration>(loopDuration));
    }

private: /* Methods: */

    template <typename ... Args>
    inline size_t waitSpaceAvailable_(Args && ... args) const {
        return this->bufferSize() - waitAvailable<HaveSpaceTest>(
                    std::forward<Args>(args)...);
    }

    template <typename ... Args>
    inline size_t waitSpaceAvailable_2(size_t & availableUntilBufferEnd,
                                       Args && ... args) const
    {
        const size_t available =
                waitAvailable<HaveSpaceTest>(std::forward<Args>(args)...);
        availableUntilBufferEnd = this->bufferSize()
                                  - std::max(available, this->writeOffset());
        return this->bufferSize() - available;
    }

    template <typename ... Args>
    inline size_t waitSpaceAvailableUntilBufferEnd_(Args && ... args) const {
        return this->bufferSize()
               - std::max(waitAvailable<HaveSpaceTest>(
                              std::forward<Args>(args)...),
                          this->writeOffset());
    }

    template <typename ... Args>
    inline size_t waitDataAvailable_(
            size_t & availableUntilBufferEnd,
            Args && ... args) const
    {
        const size_t toBufferEnd = (this->bufferSize() - this->readOffset());
        const size_t available =
                waitAvailable<HaveDataTest>(std::forward<Args>(args)...);
        availableUntilBufferEnd = std::min(available, toBufferEnd);
        return available;
    }

    template <typename ... Args>
    inline size_t waitDataAvailableUntilBufferEnd_(Args && ... args) const {
        const size_t toBufferEnd = (this->bufferSize() - this->readOffset());
        return std::min(waitAvailable<HaveDataTest>(
                            std::forward<Args>(args)...),
                        toBufferEnd);
    }

    template <typename Condition>
    inline size_t waitAvailable() const {
        Locking & lockingImpl = this->m_locking;
        typename Locking::ScopedReadLock lock(lockingImpl);
        while (!Condition::test(lockingImpl.dataAvailableNoLocking(),
                                this->bufferSize()))
            lockingImpl.wait(lock);
        return lockingImpl.dataAvailableNoLocking();
    }

    template <typename Condition, typename StopTest, typename LoopDuration>
    inline size_t waitAvailable(StopTest stopTest,
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

template <typename T, typename Locking = CircBufferDefaultLocking>
using CircBufferSCSP = Detail::CircBufferBase2<T, Locking>;

} /* namespace sharemind { */

#endif /* SHAREMIND_CIRCBUFFERSCSP_H */
