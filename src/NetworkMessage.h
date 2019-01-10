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
#include <iterator>
#include <limits>
#include <new>
#include <sharemind/EndianMacros.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "detail/ExceptionMacros.h"
#include "Exception.h"
#include "SignedToUnsigned.h"


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
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(
            LengthError,
            StringLengthError,
            "Outgoing message string size exceeds fundamental upper limit!");
    SHAREMIND_DETAIL_DEFINE_EXCEPTION_CONST_MSG(
            LengthError,
            BlockLengthError,
            "Outgoing message block size exceeds fundamental upper limit!");

public: /* Methods: */

    virtual inline ~NetworkMessage() noexcept {}

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
    using BlockSizeType = std::uint32_t;
    static_assert(sizeof(BlockSizeType) <= sizeof(std::size_t),
                  "BlockSizeType too wide!");
    using StringSizeType = std::uint16_t;
    static_assert(sizeof(StringSizeType) <= sizeof(std::size_t),
                  "StringSizeType too wide!");

public: /* Methods: */

    inline void const * getData() const noexcept { return data; }
    inline std::size_t getSize() const noexcept { return size; }
    inline std::size_t getOffset() const noexcept { return m_offset; }

    inline void const * getPtr() const noexcept
    { return static_cast<char const *>(data) + m_offset; }

    inline std::size_t getRemainingSize() const noexcept { return size - m_offset; }

    inline bool seek(std::size_t pos) noexcept;

private: /* Methods: */

    inline SeekableNetworkMessage() noexcept;
    SeekableNetworkMessage(SeekableNetworkMessage const &) = delete;
    inline SeekableNetworkMessage(void const * const data,
                                  std::size_t const size) noexcept;

    SeekableNetworkMessage & operator=(SeekableNetworkMessage const &) = delete;

    template <typename T>
    static inline std::size_t maxItemsInSizeT() noexcept;

private: /* Fields: */

    std::size_t m_offset;

}; /* class SeekableNetworkMessage { */

class IncomingNetworkMessage: public SeekableNetworkMessage {

public: /* Methods: */

    inline IncomingNetworkMessage(void const * const data,
                                  std::size_t const size) noexcept;

    inline bool readBlock(void * begin, void * end)
            noexcept __attribute__ ((warn_unused_result));

    template <typename T>
    inline bool readBlock(T * begin, T * end)
            noexcept __attribute__ ((warn_unused_result));

    inline bool readBytes(void * buffer, std::size_t size) noexcept;

    inline bool readEmptyBlock() noexcept __attribute__ ((warn_unused_result));

    template <typename T>
    inline bool read(T & val) noexcept __attribute__ ((warn_unused_result));

    inline bool read(std::string & val) __attribute__ ((warn_unused_result));

    inline bool read(bool & val) noexcept __attribute__ ((warn_unused_result));

    template <typename SerializedSizeType = DefaultSizeType,
              typename OverflowException = std::bad_array_new_length,
              typename SizeType>
    inline bool readSize(SizeType & size)
            noexcept(false) __attribute__ ((warn_unused_result));

    template <typename SerializedSizeType = DefaultSizeType,
              typename SizeType,
              typename OverflowException>
    inline bool readSize(SizeType & size,
                         OverflowException && overflowException)
            noexcept(false) __attribute__ ((warn_unused_result));

}; /* class IncomingNetworkMessage { */

class OutgoingNetworkMessage: public SeekableNetworkMessage {

public: /* Methods: */

    OutgoingNetworkMessage(OutgoingNetworkMessage const &) = delete;
    OutgoingNetworkMessage & operator=(OutgoingNetworkMessage const &) = delete;

    inline OutgoingNetworkMessage() noexcept {}
    inline ~OutgoingNetworkMessage() noexcept override
    { free(const_cast<void *>(this->data)); }

    inline void rewind() noexcept { m_offset = 0u; }

    inline void write(bool val) noexcept(false);
    inline void write(char const * val) noexcept(false);
    inline void write(char const * val, std::size_t const size) noexcept(false);
    inline void write(std::string const & val) noexcept(false);
    inline void write(NetworkMessage const & val) noexcept(false);

    template<typename T>
    inline void write(T && val) noexcept(false);

    template <typename SerializedSizeType = DefaultSizeType, typename SizeType>
    inline void writeSize(SizeType size) noexcept(false);

    inline void writeBlock(void const * begin, void const * end)
            noexcept(false);

    template <typename T>
    inline void writeBlock(T const * begin, T const * end) noexcept(false);
    inline void writeEmptyBlock() noexcept(false);

    inline void writeBytes(void const * data, std::size_t const bytes)
            noexcept(false);

private: /* Methods: */

    inline std::size_t spaceLeft() const noexcept
    { return std::numeric_limits<std::size_t>::max() - m_offset; }

    inline void addBytes(std::size_t const bytes) noexcept(false);
    inline void pokeBytes(void const * data, std::size_t const bytes) noexcept;

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
    this->data = nullptr;
    this->size = 0u;
}

inline SeekableNetworkMessage::SeekableNetworkMessage(void const * const data,
                                                      std::size_t const size)
        noexcept
    : m_offset{0u}
{
    assert(data || size == 0u);
    this->data = data;
    this->size = size;
}

inline bool SeekableNetworkMessage::seek(std::size_t pos) noexcept {
    if (pos >= size)
        return false;

    m_offset = pos;
    return true;
}

template <typename T>
inline std::size_t SeekableNetworkMessage::maxItemsInSizeT() noexcept {
    static_assert(sizeof(T) > 0u, "T must be a non-empty type!");
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}

/*******************************************************************************
  Implementations of IncomingNetworkMessage methods:
*******************************************************************************/

inline IncomingNetworkMessage::IncomingNetworkMessage(
        void const * const data,
        std::size_t const size) noexcept
    : SeekableNetworkMessage{((void) assert(data || size == 0u), data), size}
{}

template <typename T>
inline bool IncomingNetworkMessage::readBlock(T * begin, T * end) noexcept {
    assert(this->data);
    assert(m_offset <= this->size);

    std::size_t const rollbackOffset = m_offset;

    BlockSizeType size;
    if (!read(size))
        return false;
    size = sharemind::littleEndianToHost(size);

    if ((size > maxItemsInSizeT<T>()) // overflow check
        || (size > signedToUnsigned(std::distance(begin, end)))
        || (this->size - m_offset < sizeof(T) * size))
    {
        m_offset = rollbackOffset;
        return false;
    }

    memcpy(begin,
           static_cast<char const *>(this->data) + m_offset,
           sizeof(T) * size);
    m_offset += sizeof(T) * size;
    return true;
}

inline bool IncomingNetworkMessage::readBlock(void * begin, void * end) noexcept
{ return readBlock(static_cast<char *>(begin), static_cast<char *>(end)); }

inline bool IncomingNetworkMessage::readBytes(void * buffer, std::size_t size)
        noexcept
{
    assert(this->data);
    assert(m_offset <= this->size);

    if (this->size - m_offset < size)
        return false;

    memcpy(buffer, static_cast<char const *>(this->data) + m_offset, size);
    m_offset += size;
    return true;
}

inline bool IncomingNetworkMessage::readEmptyBlock() noexcept {
    assert(this->data);
    assert(m_offset <= this->size);

    BlockSizeType size = 0;
    if (! read (size))
        return false;
    size = sharemind::littleEndianToHost(size);
    return size == 0;
}

template <typename T>
inline bool IncomingNetworkMessage::read(T & val) noexcept {
    assert(this->data);
    assert(m_offset <= this->size);

    if (this->size - m_offset < sizeof(T))
        return false;

    if (sizeof(T) > 0u) {
        memcpy(&val,
               static_cast<char const *>(this->data) + m_offset,
               sizeof(T));
        m_offset += sizeof(T);
    }

    return true;
}

inline bool IncomingNetworkMessage::read(std::string & val) {
    assert(this->data);
    assert(m_offset <= this->size);

    std::size_t const rollbackOffset = m_offset;

    StringSizeType length;
    if (!read(length))
        return false;
    length = sharemind::littleEndianToHost(length);

    if (length > this->size - m_offset) {
        m_offset = rollbackOffset;
        return false;
    }

    if (length > 0u) {
        char const * const s = static_cast<char const *>(this->data) + m_offset;
        try {
            val.assign(s, s + length);
        } catch (...) {
            m_offset = rollbackOffset;
            throw;
        }
        m_offset += length;
    } else {
        val.clear();
    }
    return true;
}

inline bool IncomingNetworkMessage::read(bool & val) noexcept {
    assert(this->data);
    assert(m_offset <= this->size);

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
inline bool IncomingNetworkMessage::readSize(SizeType & size)
        noexcept(false)
{
    SerializedSizeType rawSize;
    auto const r(read(rawSize));
    if (r) {
        rawSize = sharemindNetToHostOrder(rawSize);
        if (!SizeTypeInfo<SizeType>::fitsValue(rawSize))
            throw OverflowException();
        size = static_cast<SizeType>(rawSize);
    }
    return r;
}

template <typename SerializedSizeType,
          typename SizeType,
          typename OverflowException>
inline bool IncomingNetworkMessage::readSize(
        SizeType & size,
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
        size = static_cast<SizeType>(rawSize);
    }
    return r;
}


/*******************************************************************************
  Implementations of OutgoingNetworkMessage methods:
*******************************************************************************/

inline void OutgoingNetworkMessage::write(bool val) noexcept(false) {
    std::uint8_t const value = static_cast<std::uint8_t>(val ? 1u : 0u);
    writeBytes(&value, sizeof(value));
}

inline void OutgoingNetworkMessage::write(char const * val) noexcept(false)
{ write(val, strlen(val)); }

inline void OutgoingNetworkMessage::write(char const * val,
                                          std::size_t const size) noexcept(false)
{
    if (size > 0u) {
        if (!SizeTypeInfo<StringSizeType>::fitsValue(size))
            throw StringLengthError{};
        if (std::numeric_limits<std::size_t>::max() - size
            < sizeof(StringSizeType))
            throw MessageLengthError{};
        addBytes(sizeof(StringSizeType) + size);
        StringSizeType const s =
                sharemind::hostToLittleEndian(static_cast<StringSizeType>(size));
        pokeBytes(&s, sizeof(s));
        pokeBytes(val, size);
    } else {
        addBytes(sizeof(StringSizeType));
        static StringSizeType const zeroSize =
                sharemind::hostToLittleEndian(static_cast<StringSizeType>(0u));
        pokeBytes(&zeroSize, sizeof(zeroSize));
    }
}

inline void OutgoingNetworkMessage::write(std::string const & val)
        noexcept(false)
{ write(val.c_str(), val.length()); }

inline void OutgoingNetworkMessage::write(NetworkMessage const & message)
        noexcept(false)
{ writeBytes(message.data, message.size); }

template <typename T>
inline void OutgoingNetworkMessage::write(T && val) noexcept(false)
{ writeBytes(&val, sizeof(T)); }

inline void OutgoingNetworkMessage::writeEmptyBlock() noexcept(false) {
    static BlockSizeType const zero =
            sharemind::hostToLittleEndian(static_cast<BlockSizeType>(0u));
    writeBytes(&zero, sizeof(zero));
}

template <typename SerializedSizeType, typename SizeType>
inline void OutgoingNetworkMessage::writeSize(SizeType size) noexcept(false) {
    static_assert(std::is_unsigned<SerializedSizeType>::value, "");
    static_assert(std::is_unsigned<SizeType>::value, "");
    static_assert(std::numeric_limits<decltype(size)>::max()
                  <= std::numeric_limits<SerializedSizeType>::max(), "");
    write(hostToSharemindNetOrder(static_cast<SerializedSizeType>(size)));
}

template <typename T>
inline void OutgoingNetworkMessage::writeBlock(T const * begin, T const * end)
        noexcept(false)
{
    assert(begin <= end);
    std::size_t const numItems = signedToUnsigned(std::distance(begin, end));
    if (numItems > 0u) {
        if (!SizeTypeInfo<BlockSizeType>::fitsValue(numItems))
            throw BlockLengthError{};
        if (numItems > maxItemsInSizeT<T>())
            throw MessageLengthError{};
        std::size_t const totalDataSize = numItems * sizeof(T);
        if (std::numeric_limits<std::size_t>::max() - sizeof(BlockSizeType)
            < totalDataSize)
            throw MessageLengthError{};
        std::size_t const totalSize = totalDataSize + sizeof(BlockSizeType);
        if (totalSize > spaceLeft())
            throw MessageLengthError{};

        addBytes(totalSize);
        BlockSizeType const  serializedNumItems =
                sharemind::hostToLittleEndian(
                    static_cast<BlockSizeType>(numItems));
        pokeBytes(&serializedNumItems, sizeof(BlockSizeType));
        pokeBytes(begin, totalDataSize);
    } else {
        addBytes(sizeof(BlockSizeType));
        static BlockSizeType const zeroItems =
                sharemind::hostToLittleEndian(static_cast<BlockSizeType>(0u));
        pokeBytes(&zeroItems, sizeof(zeroItems));
    }
}

inline void OutgoingNetworkMessage::writeBlock(void const * const begin,
                                               void const * const end)
        noexcept(false)
{ writeBlock(static_cast<char const *>(begin), static_cast<char const *>(end));}

inline void OutgoingNetworkMessage::writeBytes(void const * data,
                                               std::size_t const bytes)
        noexcept(false)
{
    if (bytes == 0u)
        return;
    if (bytes > spaceLeft())
        throw MessageLengthError{};

    addBytes(bytes);
    pokeBytes(data, bytes);
}

inline void OutgoingNetworkMessage::addBytes(std::size_t const bytes)
        noexcept(false)
{
    assert(bytes > 0u);
    assert(bytes <= std::numeric_limits<std::size_t>::max() - m_offset);
    assert(this->size == m_offset); // All data previously poked

    std::size_t const newSize = this->size + bytes;
    void * const newData = realloc(const_cast<void *>(this->data), newSize);
    if (!newData)
        throw std::bad_alloc{};

    this->data = newData;
    this->size = newSize;
}

inline void OutgoingNetworkMessage::pokeBytes(void const * data,
                                              std::size_t const bytes) noexcept
{
    assert(data || bytes == 0u);
    assert(this->size - m_offset >= bytes);

    memcpy(static_cast<char *>(const_cast<void *>(this->data)) + m_offset,
           data,
           bytes);
    m_offset += bytes;
}

} /* namespace sharemind */

#endif /* SHAREMIND_NETWORKMESSAGE_H */
