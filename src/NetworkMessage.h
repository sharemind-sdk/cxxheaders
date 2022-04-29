/*
* This file is a part of the Sharemind framework.
* Copyright (C) Cybernetica AS
*
* All rights are reserved. Reproduction in whole or part is prohibited
* without the written consent of the copyright owner. The usage of this
* code is subject to the appropriate license agreement.
*/

#ifndef SHAREMIND_NETWORKMESSAGE_H
#define SHAREMIND_NETWORKMESSAGE_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <sharemind/EndianMacros.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "detail/ExceptionMacros.h"
#include "Exception.h"
#include "PotentiallyVoidTypeInfo.h"


namespace sharemind {

struct NetworkMessagePayload {
    void const * data;
    std::size_t size;
};

class NetworkMessage: public NetworkMessagePayload {

public: /* Types: */

    SHAREMIND_DETAIL_DEFINE_EXCEPTION(sharemind::Exception, Exception);
    SHAREMIND_DETAIL_DEFINE_EXCEPTION(Exception, LengthError);
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(
            LengthError,
            MessageLengthError,
            "Outgoing message size exceeds fundamental upper limit!");

public: /* Methods: */

    virtual ~NetworkMessage() noexcept {}

};

class SeekableNetworkMessage: public NetworkMessage {

    friend class IncomingNetworkMessage;
    friend class OutgoingNetworkMessage;

private: /* Types: */

    template <typename T>
    struct SizeTypeInfo {

        /* Methods: */

        template <typename U>
        static inline bool fitsValue(U value) noexcept;

    };

public: /* Types: */

    using DefaultSizeType = std::uint64_t;

public: /* Methods: */

    void const * getData() const noexcept { return data; }
    std::size_t getSize() const noexcept { return size; }
    std::size_t getOffset() const noexcept { return m_offset; }

    void const * getPtr() const noexcept
    { return ptrAdd(data, m_offset); }

    std::size_t getRemainingSize() const noexcept { return size - m_offset; }

    inline bool seek(std::size_t pos) noexcept;

private: /* Methods: */

    inline SeekableNetworkMessage() noexcept;
    SeekableNetworkMessage(SeekableNetworkMessage const &) = delete;
    inline SeekableNetworkMessage(void const * const data_,
                                  std::size_t const size_) noexcept;

    SeekableNetworkMessage & operator=(SeekableNetworkMessage const &) = delete;

private: /* Fields: */

    std::size_t m_offset;

}; /* class SeekableNetworkMessage { */

class IncomingNetworkMessage: public SeekableNetworkMessage {

public: /* Methods: */

    inline IncomingNetworkMessage(void const * const data_,
                                  std::size_t const size_) noexcept;

    inline bool readBytes(void * buffer, std::size_t size_) noexcept;

    template <typename T>
    inline bool read(T & val) noexcept __attribute__ ((warn_unused_result));

    inline bool read(std::string & val) __attribute__ ((warn_unused_result));

    inline bool read(bool & val) noexcept __attribute__ ((warn_unused_result));

    template <typename SerializedSizeType = DefaultSizeType,
              typename OverflowException = std::bad_array_new_length,
              typename SizeType>
    inline bool readSize(SizeType & size_)
            noexcept(false) __attribute__ ((warn_unused_result));

    template <typename SerializedSizeType = DefaultSizeType,
              typename SizeType,
              typename OverflowException>
    inline bool readSize(SizeType & size_,
                         OverflowException && overflowException)
            noexcept(false) __attribute__ ((warn_unused_result));

    template <typename T, typename SizeType>
    inline bool readArray(T * data_, SizeType const size_)
            noexcept __attribute__ ((warn_unused_result));

}; /* class IncomingNetworkMessage { */

class OutgoingNetworkMessage: public SeekableNetworkMessage {

public: /* Methods: */

    OutgoingNetworkMessage(OutgoingNetworkMessage const &) = delete;
    OutgoingNetworkMessage & operator=(OutgoingNetworkMessage const &) = delete;

    OutgoingNetworkMessage() noexcept {}
    ~OutgoingNetworkMessage() noexcept override
    { std::free(const_cast<void *>(data)); }

    void rewind() noexcept { m_offset = 0u; }

    inline void write(bool val) noexcept(false);
    inline void write(char const * val) noexcept(false);
    inline void write(char const * val, std::size_t const size_) noexcept(false);
    inline void write(std::string const & val) noexcept(false);
    inline void write(NetworkMessage const & val) noexcept(false);

    template<typename T>
    inline void write(T && val) noexcept(false);

    template <typename SerializedSizeType = DefaultSizeType, typename SizeType>
    inline void writeSize(SizeType size_) noexcept(false);

    template <typename T, typename SizeType>
    inline void writeArray(T const * data_, SizeType size_) noexcept(false);

    template <typename SerializedSizeType = DefaultSizeType,
              typename T,
              typename SizeType>
    inline void writeSizeAndArray(T const * data_, SizeType size_)
            noexcept(false);

    inline void writeBytes(void const * data_, std::size_t const bytes)
            noexcept(false);

private: /* Methods: */

    std::size_t spaceLeft() const noexcept
    { return std::numeric_limits<std::size_t>::max() - m_offset; }

    inline void addBytes(std::size_t const bytes) noexcept(false);
    inline void pokeBytes(void const * data_, std::size_t const bytes) noexcept;

}; /* class OutgoingNetworkMessage { */


/*******************************************************************************
  Implementations of SeekableNetworkMessage methods:
*******************************************************************************/

template <typename T>
template <typename U>
inline bool SeekableNetworkMessage::SizeTypeInfo<T>::fitsValue(U value) noexcept
{
    static_assert(std::numeric_limits<T>::is_specialized,
                  "T must be specialized!");
    static_assert(std::numeric_limits<U>::is_specialized,
                  "U must be specialized!");
    return std::numeric_limits<T>::max() >= value;
}

inline SeekableNetworkMessage::SeekableNetworkMessage() noexcept
    : m_offset{0u}
{
    data = nullptr;
    size = 0u;
}

inline SeekableNetworkMessage::SeekableNetworkMessage(void const * const data_,
                                                      std::size_t const size_)
        noexcept
    : m_offset{0u}
{
    assert(data_ || size_ == 0u);
    data = data_;
    size = size_;
}

inline bool SeekableNetworkMessage::seek(std::size_t pos) noexcept {
    if (pos >= size)
        return false;

    m_offset = pos;
    return true;
}


/*******************************************************************************
  Implementations of IncomingNetworkMessage methods:
*******************************************************************************/

inline IncomingNetworkMessage::IncomingNetworkMessage(
        void const * const data_,
        std::size_t const size_) noexcept
    : SeekableNetworkMessage{((void) assert(data_ || size_ == 0u), data_),
                             size_}
{}

inline bool IncomingNetworkMessage::readBytes(void * buffer, std::size_t size_)
        noexcept
{
    assert(data);
    assert(m_offset <= size);

    if (size - m_offset < size_)
        return false;

    std::memcpy(buffer, ptrAdd(data, m_offset), size_);
    m_offset += size_;
    return true;
}


template <typename T>
inline bool IncomingNetworkMessage::read(T & val) noexcept {
    assert(data);
    assert(m_offset <= size);

    if (size - m_offset < sizeof(T))
        return false;

    if (sizeof(T) > 0u) {
        std::memcpy(&val, ptrAdd(data, m_offset), sizeof(T));
        m_offset += sizeof(T);
    }

    return true;
}

inline bool IncomingNetworkMessage::read(std::string & val) {
    assert(data);
    assert(m_offset <= size);

    auto const rollbackOffset(m_offset);

    decltype(val.size()) length;
    if (!readSize(length))
        return false;

    if (length <= 0u) {
        val.clear();
        return true;
    }

    if (length > size - m_offset) {
        m_offset = rollbackOffset;
        return false;
    }

    auto const s = static_cast<char const *>(data) + m_offset;
    try {
        val.assign(s, s + length);
    } catch (...) {
        m_offset = rollbackOffset;
        throw;
    }
    m_offset += length;
    return true;
}

inline bool IncomingNetworkMessage::read(bool & val) noexcept {
    assert(data);
    assert(m_offset <= size);

    std::size_t const rollbackOffset = m_offset;

    std::uint8_t temp;
    if (!read(temp))
        return false;

    switch (temp) {
        case 0u:
            val = false;
            return true;
        case 1u:
            val = true;
            return true;
        default:
            m_offset = rollbackOffset;
            return false;
    }
}

template <typename SerializedSizeType,
          typename OverflowException,
          typename SizeType>
inline bool IncomingNetworkMessage::readSize(SizeType & outSize)
        noexcept(false)
{
    SerializedSizeType rawSize;
    auto const r(read(rawSize));
    if (r) {
        rawSize = sharemindNetToHostOrder(rawSize);
        if (!SizeTypeInfo<SizeType>::fitsValue(rawSize))
            throw OverflowException();
        outSize = static_cast<SizeType>(rawSize);
    }
    return r;
}

template <typename SerializedSizeType,
          typename SizeType,
          typename OverflowException>
inline bool IncomingNetworkMessage::readSize(
        SizeType & outSize,
        OverflowException && overflowException)
        noexcept(false)
{
    static_assert(std::is_unsigned<SerializedSizeType>::value, "");
    static_assert(std::is_unsigned<SizeType>::value, "");

    SerializedSizeType rawSize;
    auto const r(read(rawSize));
    if (r) {
        rawSize = sharemindNetToHostOrder(rawSize);
        if (!SizeTypeInfo<SizeType>::fitsValue(rawSize))
            throw std::forward<OverflowException>(overflowException);
        outSize = static_cast<SizeType>(rawSize);
    }
    return r;
}

template <typename T, typename SizeType>
inline bool IncomingNetworkMessage::readArray(T * data_, SizeType const size_)
        noexcept
{
    static_assert(std::is_unsigned<SizeType>::value, "");

    assert(data);
    assert(m_offset <= size);

    using S = decltype(sizeof(T) * size_);
    static_assert(std::numeric_limits<S>::max()
                  >= std::numeric_limits<std::size_t>::max(), "");
    if (std::numeric_limits<S>::max() / sizeof(T) < size_)
        return false;
    auto const sizeInBytes(sizeof(T) * size_);
    if (getRemainingSize() < sizeInBytes)
        return false;

    std::memcpy(data_, ptrAdd(data, m_offset), sizeInBytes);
    m_offset += sizeInBytes;
    return true;
}


/*******************************************************************************
  Implementations of OutgoingNetworkMessage methods:
*******************************************************************************/

inline void OutgoingNetworkMessage::write(bool val) noexcept(false) {
    std::uint8_t const value = static_cast<std::uint8_t>(val ? 1u : 0u);
    writeBytes(&value, sizeof(value));
}

inline void OutgoingNetworkMessage::write(char const * val) noexcept(false)
{ writeSizeAndArray(val, std::strlen(val)); }

inline void OutgoingNetworkMessage::write(char const * val,
                                          std::size_t const size_)
        noexcept(false)
{ writeSizeAndArray(val, size_); }

inline void OutgoingNetworkMessage::write(std::string const & val)
        noexcept(false)
{ writeSizeAndArray(val.c_str(), val.length()); }

inline void OutgoingNetworkMessage::write(NetworkMessage const & message)
        noexcept(false)
{ writeBytes(message.data, message.size); }

template <typename T>
inline void OutgoingNetworkMessage::write(T && val) noexcept(false)
{ writeBytes(&val, sizeof(T)); }

template <typename SerializedSizeType, typename SizeType>
inline void OutgoingNetworkMessage::writeSize(SizeType size_) noexcept(false) {
    static_assert(std::is_unsigned<SerializedSizeType>::value, "");
    static_assert(std::is_unsigned<SizeType>::value, "");
    static_assert(std::numeric_limits<decltype(size_)>::max()
                  <= std::numeric_limits<SerializedSizeType>::max(), "");
    write(hostToSharemindNetOrder(static_cast<SerializedSizeType>(size_)));
}

template <typename T, typename SizeType>
inline void OutgoingNetworkMessage::writeArray(T const * data_, SizeType size_)
        noexcept(false)
{
    static_assert(std::is_unsigned<SizeType>::value, "");

    if (size_ <= 0u)
        return;
    if (spaceLeft() / sizeof(T) < size_)
        throw MessageLengthError();
    auto const arraySizeInBytes(size_ * sizeof(T));
    addBytes(arraySizeInBytes);
    pokeBytes(data_, arraySizeInBytes);
}

template <typename SerializedSizeType, typename T, typename SizeType>
inline void OutgoingNetworkMessage::writeSizeAndArray(T const * data_,
                                                      SizeType size_)
        noexcept(false)
{
    static_assert(std::is_unsigned<SerializedSizeType>::value, "");
    static_assert(std::is_unsigned<SizeType>::value, "");

    if (size_ <= 0)
        return writeSize<SerializedSizeType, SizeType>(size_);

    {
        auto bytesLeft(spaceLeft());
        if (sizeof(SerializedSizeType) > bytesLeft)
            throw MessageLengthError();
        bytesLeft -= sizeof(SerializedSizeType);
        if (bytesLeft / sizeof(T) < size_)
            throw MessageLengthError();
    }
    auto const arraySizeInBytes(size_ * sizeof(T));

    addBytes(sizeof(SerializedSizeType) + arraySizeInBytes);
    SerializedSizeType const serializedSize(
                hostToSharemindNetOrder(
                    static_cast<SerializedSizeType>(size_)));
    pokeBytes(&serializedSize, sizeof(serializedSize));
    pokeBytes(data_, arraySizeInBytes);
}

inline void OutgoingNetworkMessage::writeBytes(void const * data_,
                                               std::size_t const bytes)
        noexcept(false)
{
    if (bytes == 0u)
        return;
    if (bytes > spaceLeft())
        throw MessageLengthError{};

    addBytes(bytes);
    pokeBytes(data_, bytes);
}

inline void OutgoingNetworkMessage::addBytes(std::size_t const bytes)
        noexcept(false)
{
    assert(bytes > 0u);
    assert(bytes <= std::numeric_limits<std::size_t>::max() - m_offset);
    assert(size == m_offset); // All data previously poked

    std::size_t const newSize = size + bytes;
    void * const newData = std::realloc(const_cast<void *>(data), newSize);
    if (!newData)
        throw std::bad_alloc{};

    data = newData;
    size = newSize;
}

inline void OutgoingNetworkMessage::pokeBytes(void const * data_,
                                              std::size_t const bytes) noexcept
{
    assert(data_ || bytes == 0u);
    assert(size - m_offset >= bytes);

    std::memcpy(ptrAdd(const_cast<void *>(data), m_offset), data_, bytes);
    m_offset += bytes;
}

} /* namespace sharemind */

#endif /* SHAREMIND_NETWORKMESSAGE_H */
