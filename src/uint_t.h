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

#ifndef SHAREMIND_UINT_H
#define SHAREMIND_UINT_H

#include "integer_demotion.h"
#include "uint128_t.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include "Min.h"


namespace sharemind {

namespace detail {

template <typename T> struct double_size {};
template <> struct double_size<uint8_t>  { using type_t = uint16_t; };
template <> struct double_size<uint16_t> { using type_t = uint32_t; };
template <> struct double_size<uint32_t> { using type_t = uint64_t; };

} /* namespace detail { */

template <typename T> struct mul_t { T hi, lo; };

/**
 * Multiply two unsigned integers and return both high and low bits of the result.
 */
template <typename T>
inline mul_t<T> multHiLo(T u, T v) {
    using double_T = typename detail::double_size<T>::type_t;
    const double_T uu = u, vv = v;
    const double_T resValue = uu*vv;
    mul_t<T> res;
    res.hi = resValue >> sizeof(T)*8;
    res.lo = resValue;
    return res;
}


/**
 * The code is from article:
 *  Montgomery Multiplication
 *  By Henry S. Warren, Jr.
 *
 * The uncummented version is quite efficient. The mulq based version is only about 1.2
 * times faster for the use in my multiplication.
 */
template <>
inline mul_t<uint64_t> multHiLo<uint64_t>(uint64_t u, uint64_t v) {

    uint64_t u0, u1, v0, v1, k, t;
    uint64_t w0, w1, w2;
    u1 = u >> 32; u0 = u & 0xFFFFFFFF;
    v1 = v >> 32; v0 = v & 0xFFFFFFFF;
    t = u0*v0;
    w0 = t & 0xFFFFFFFF;
    k = t >> 32;
    t = u1*v0 + k;
    w1 = t & 0xFFFFFFFF;
    w2 = t >> 32;
    t = u0*v1 + w1;
    k = t >> 32;
    mul_t<uint64_t> res;
    res.lo = (t << 32) + w0;
    res.hi = u1*v1 + w2 + k;
    return res;

    /*
     * Version using 128 bit integers.
     * Both gcc and clang can optimize this to a single mulq. However, because
     * inline assembly hides other optimization opportunities this version is
     * better. Consider enabling this version if the compiler supports 128 bit
     * integers.
     */

//    const unsigned __int128 uu = u;
//    const unsigned __int128 vv = v;
//    const unsigned __int128 r = uu*vv;
//    mul_t<uint64_t> res;
//    res.lo = r;
//    res.hi = r >> 64;
//    return res;
}


/**
 * Some implementation details for proper memory management.
 * For high enough bit values we should allocate the memory dynamically.
 * Otherwise issues with stack blowup may easily happen.
 */
template <unsigned N, typename B>
class uint_detail {
public: /* Types: */
    using block_t = B;

    static constexpr std::size_t bytes_in_block = sizeof(block_t);
    static constexpr std::size_t num_of_bits = N;
    static constexpr std::size_t bits_in_block = bytes_in_block*8u;
    static constexpr std::size_t num_of_bytes = (num_of_bits + 7u) / 8u;
    static constexpr std::size_t num_of_blocks =
            (num_of_bytes + bytes_in_block - 1u) / bytes_in_block;

public: /* Methods: */

    inline block_t& block (std::size_t const i) {
        assert (i < num_of_blocks);
        return m_blocks[i];
    }

    inline block_t block (std::size_t const i) const {
        assert (i < num_of_blocks);
        return m_blocks[i];
    }

    inline block_t* begin () { return &m_blocks[0]; }
    inline block_t* end () { return begin() + num_of_blocks; }
    inline const block_t* begin () const { return &m_blocks[0]; }
    inline const block_t* end () const { return begin() + num_of_blocks; }

private: /* Fields: */
    block_t m_blocks[num_of_blocks];
};

/**
 * Arbitrary precision static length unsigned integers.
 * \invariant Unused bits are always set to zero.
 */
template <unsigned N, typename B = uint64_t>
class uint_t : public uint_detail<N, B> {
public:

    static_assert (std::is_unsigned<B>::value && ! std::is_same<B, bool>::value,
                   "Block size of uint_t must be unsigned non-boolean type.");

    using block_t = typename uint_detail<N, B>::block_t;
    using uint_detail<N, B>::num_of_bits;
    using uint_detail<N, B>::num_of_blocks;
    using uint_detail<N, B>::bits_in_block;
    using uint_detail<N, B>::block;
    using uint_detail<N, B>::begin;
    using uint_detail<N, B>::end;

    // This is a mask for the last block. In the mask only "useful" bits are set.
    static constexpr block_t last_block_mask = block_t(~block_t(0)) >> (bits_in_block*num_of_blocks - num_of_bits);

public: /* Types: */

    struct div_t { uint_t quot, rem; };

    class division_by_zero_error : public std::runtime_error {
    public: /* Methods: */
        division_by_zero_error ()
            : std::runtime_error ("Division by zero")
        { }
    };

    class bit_reference {
    private:
        void operator& (); // DO NOT IMPLEMENT
    public: /* Methods: */
        bit_reference (block_t& block, block_t pos)
            : m_block (block)
            , m_mask (block_t(1) << pos)
        { }

        operator bool () const { return (m_block & m_mask) != 0; }
        bool operator ~ () const { return (m_block & m_mask) == 0; }
        bit_reference& flip() { flip_bit_ (); return *this; }

        bit_reference& operator = (bool x)            { assign_bit_ (x);   return *this; }
        bit_reference& operator = (bit_reference rhs) { assign_bit_ (rhs); return *this; }

        bit_reference& operator |= (bool x) { if  (x) set_bit_ ();   return *this; }
        bit_reference& operator &= (bool x) { if (!x) reset_bit_ (); return *this; }
        bit_reference& operator ^= (bool x) { if  (x) flip_bit_ ();  return *this; }
        bit_reference& operator -= (bool x) { if  (x) reset_bit_ (); return *this; }

    private: /* Methods: */
        inline void set_bit_ () const { m_block |= m_mask; }
        inline void reset_bit_ () const { m_block &= ~m_mask; }
        inline void flip_bit_ () const { m_block ^= m_mask; }
        inline void assign_bit_ (bool x) const { x ? set_bit_ () : reset_bit_ (); }

    public: /* Fields: */
        block_t&      m_block;
        const block_t m_mask;
    };

public: /* Methods: */


    /*********************************
     * Constructors/destructors etc. *
     *********************************/


    uint_t() {
        std::fill(begin(), end(), block_t(0));
    }

    uint_t (const uint_t& other) {
        std::copy(other.begin(), other.end(), begin());
    }

    uint_t (uint_t&& other) {
        std::copy(other.begin(), other.end(), begin());
    }

    template <typename Generator>
    uint_t (Generator && generator) {
        constructor_ (std::forward<Generator>(generator));
    }

    template <typename T>
    /* implicit */ uint_t (const T& val) {
        constructor_ (val);
    }

    uint_t& operator = (const uint_t& rhs) {
        if (this != &rhs) {
            std::copy (rhs.begin(), rhs.end(), begin());
        }

        return *this;
    }

    inline uint_t & operator=(uint_t && rhs)
    { return this->operator=(rhs); } // Use copy assign

    template <typename T>
    uint_t& operator = (const T& rhs) {
        *this = uint_t(rhs);
        return *this;
    }


    /*******************
     * Unary operators *
     *******************/


    friend bool operator ! (uint_t x) {
        block_t acc = 0;
        for (std::size_t i = 0u; i < num_of_blocks; ++i)
            acc |= x.block(i);

        return !acc;
    }

    friend uint_t operator ~ (uint_t x) {
        uint_t result;
        for (std::size_t i = 0u; i < num_of_blocks; ++i)
            result.block(i) = ~x.block(i);

        result.clear_unused_bits_();
        return result;
    }

    friend uint_t operator - (uint_t x) {
        return 0 - x;
    }


    /**************
     * Bit access *
     **************/


    inline bool operator [] (std::size_t const i) const {
        assert (i < N && "Index out of bounds.");
        std::size_t const block_index = i / bits_in_block;
        const block_t bit_index = i % bits_in_block;
        const block_t bit_mask = block_t(1) << bit_index;
        return bit_mask & block(block_index);
    }

    inline bit_reference operator [] (std::size_t const i) {
        assert (i < N && "Index out of bounds.");
        std::size_t const block_index = i / bits_in_block;
        const block_t bit_index = i % bits_in_block;
        return bit_reference(block(block_index), bit_index);
    }


    /******************************
     * Operators that modify this *
     ******************************/


    // TODO: Most of the following should be inlined manually and optimized.

    uint_t& operator ++ () { increment_(1); return *this; }
    uint_t operator ++ (int) { const uint_t t = *this; increment_(1); return t; }
    uint_t& operator -- () { decrement_(1); return *this; }
    uint_t operator -- (int) { const uint_t t = *this; decrement_(1); return t; }
    uint_t& operator &= (uint_t x) { *this = *this & x; return *this; }
    uint_t& operator ^= (uint_t x) { *this = *this ^ x; return *this; }
    uint_t& operator |= (uint_t x) { *this = *this | x; return *this; }
    uint_t& operator += (uint_t x) { *this = *this + x; return *this; }
    uint_t& operator -= (uint_t x) { *this = *this - x; return *this; }
    uint_t& operator *= (uint_t x) { *this = *this * x; return *this; }
    uint_t& operator /= (uint_t x) { *this = *this / x; return *this; }
    uint_t& operator %= (uint_t x) { *this = *this % x; return *this; }
    uint_t& operator >>= (uint_t shift) { *this = *this >> shift; return *this; }
    uint_t& operator <<= (uint_t shift) { *this = *this << shift; return *this; }
    uint_t& operator >>= (uint64_t shift) { *this = *this >> shift; return *this; }
    uint_t& operator <<= (uint64_t shift) { *this = *this << shift; return *this; }



    /**********************
     * Bitwise operations *
     **********************/


    friend uint_t operator & (uint_t x, uint_t y) {
        uint_t result;
        for (std::size_t i = 0u; i < num_of_blocks; ++i)
            result.block(i) = x.block(i) & y.block(i);
        return result;
    }

    friend uint_t operator ^ (uint_t x, uint_t y) {
        uint_t result;
        for (std::size_t i = 0u; i < num_of_blocks; ++i)
            result.block(i) = x.block(i) ^ y.block(i);
        return result;
    }

    friend uint_t operator | (uint_t x, uint_t y) {
        uint_t result;
        for (std::size_t i = 0u; i < num_of_blocks; ++ i)
            result.block(i) = x.block(i) | y.block(i);
        return result;
    }

    friend uint_t operator << (uint_t x, uint_t shift) {
        const uint64_t u64shift = shift.as_uint64_t ();
        return x << u64shift;
    }

    friend uint_t operator >> (uint_t x, uint_t shift) {
        const uint64_t u64shift = shift.as_uint64_t ();
        return x >> u64shift;
    }

    friend uint_t operator << (uint_t x, uint64_t shift) {
        assert (shift < N);

        if (shift == 0) return x;

        uint_t result;

        std::size_t const blockShift = shift / bits_in_block; // how many blocks need shifting
        std::size_t const bitShift = shift % bits_in_block; // how many bits in the block need shifting

        if (bitShift == 0) {
            result = x;
            result.shift_blocks_left_(blockShift);
            return result;
        }

        assert (0 < bitShift && bitShift < bits_in_block && "Undefined behaviour!");

        for (std::size_t i = 0u; i < blockShift; ++ i)
            result.block(i) = 0;

        result.block(blockShift) = x.block(0) << bitShift;

        for (std::size_t i = blockShift + 1; i < num_of_blocks; ++ i)
            result.block(i) =
                    (x.block(i - blockShift) << bitShift) |
                    (x.block(i - blockShift - 1) >> (bits_in_block - bitShift));

        result.clear_unused_bits_();
        return result;
    }

    friend uint_t operator >> (uint_t x, uint64_t shift) {
        assert (shift < N);

        if (shift == 0) return x;

        uint_t result;

        // how many blocks need shifting:
        std::size_t const blockShift = shift / bits_in_block;
        std::size_t const blockKeep = num_of_blocks - blockShift - 1;
        // how many bits in the block need shifting:
        std::size_t const bitShift = shift % bits_in_block;

        if (bitShift == 0) {
            result = x;
            result.shift_blocks_right_(blockShift);
            return result;
        }

        assert (0 < bitShift && bitShift < bits_in_block && "Undefined behaviour!");

        for (std::size_t i = 0u; i < blockKeep; ++i)
            result.block(i) =
                    (x.block(i + blockShift) >> bitShift) |
                    (x.block(i + blockShift + 1) << (bits_in_block - bitShift));

        result.block(blockKeep) = x.most_significant_block() >> bitShift;

        for (std::size_t i = blockKeep + 1; i < num_of_blocks; ++i)
            result.block(i) = 0;

        result.clear_unused_bits_();
        return result;
    }


    /*************************
     * Arithmetic operations *
     *************************/


    friend uint_t operator + (uint_t x, uint_t y) {
        bool carry = false;
        uint_t result;
        for (std::size_t i = 0u; i < num_of_blocks; ++i) {
            const block_t temp = x.block(i) + y.block(i) + carry;
            const block_t limit = std::min(x.block(i), y.block(i));
            result.block(i) = temp;
            carry = temp < limit || (carry && temp == limit);
        }

        result.clear_unused_bits_();
        return result;
    }

    friend uint_t operator - (uint_t x, uint_t y) {
        bool borrow = false;
        uint_t result;
        for (std::size_t i = 0u; i < num_of_blocks; ++i) {
            const block_t temp = x.block(i) - borrow;
            borrow = y.block(i) > temp || (borrow && x.block(i) == 0);
            result.block(i) = temp - y.block(i);
        }

        result.clear_unused_bits_();
        return result;
    }

    /**
     * TODO: Karatsuba for more than 300 bits or so.
     */
    friend uint_t operator * (uint_t x, uint_t y) {

        // static if
        if (num_of_blocks == 1) {
            uint_t result = x.least_significant_block() * y.least_significant_block();
            result.clear_unused_bits_();
            return result;
        }

        uint_t result = 0;
        uint_t carry = 0;
        for (std::size_t i = 0u; i < num_of_blocks; ++i) {
            for (std::size_t j = 0u; j <= i; ++j) {
                std::size_t const k = i - j;
                const mul_t<block_t> mult = multHiLo(x.block(j), y.block(k));
                uint_t temp = mult.hi;
                temp.shift_blocks_left_();
                temp.least_significant_block() |= mult.lo;
                carry += temp;
            }

            result.block(i) = carry.least_significant_block();
            carry.shift_blocks_right_();
        }

        result.clear_unused_bits_();
        return result;
    }

    friend uint_t operator / (uint_t x, uint_t y) {
        return divMod(x, y).quot;
    }

    friend uint_t operator % (uint_t x, uint_t y) {
        return divMod(x, y).rem;
    }


    /*************************
     * Relational operations *
     *************************/


    // TODO: it might be more efficient to avoid any branching
    friend bool operator == (uint_t x, uint_t y) {
        for (std::size_t i = 0u; i < num_of_blocks; ++i)
            if (x.block(i) != y.block(i))
                return false;
        return true;
    }

    friend bool operator != (uint_t x, uint_t y) { return !(x == y); }

    friend bool operator < (uint_t x, uint_t y) {
        for (std::size_t i = num_of_blocks; i --> 0u; ) {
            if (x.block(i) < y.block(i)) return true;
            if (x.block(i) > y.block(i)) return false;
        }

        return false;
    }

    friend bool operator > (uint_t x, uint_t y) { return y < x; }

    friend bool operator <= (uint_t x, uint_t y) { return !(x > y); }

    friend bool operator >= (uint_t x, uint_t y) { return !(x < y); }

    /*******************
     * Misc operations *
     *******************/

    /**
     * This is bitwise long division. Very, very, very inefficient.
     * TODO: use UNR from "Software Integer Division" by Thomas L. Rodeheffer
     */
    friend div_t divMod (uint_t x, uint_t y) {
        if (y == 0) {
            throw division_by_zero_error ();
        }

        uint_t quot, rem;
        quot = 0;
        rem = 0;

        uint_t bit = 1;
        bit <<= (num_of_bits - 1);
        while (bit != 0) {
            rem <<= 1;
            rem[0] |= !!(x & bit);
            if (rem >= y) {
                rem -= y;
                quot |= bit;
            }

            bit >>= 1;
        }

        const div_t result = { quot, rem };
        return result;
    }

    friend std::ostream& operator << (std::ostream& os, uint_t x) {
        // each octal digit represents 3 bits + room for sign:
        static constexpr std::size_t buff_size =
                1 + (num_of_bits + 2u)/3u;
        static constexpr std::size_t max_static_buffer_size = 256u;
        static constexpr bool dynamicBuffer = buff_size > max_static_buffer_size;

        const char lower[] = "0123456789abcdef";
        const char upper[] = "0123456789ABCDEF";

        char staticBuffer[dynamicBuffer ? 1 : buff_size];
        char* buff = &staticBuffer[0];
        if (dynamicBuffer) {
            buff = new char[buff_size];
        }

        if (x == 0) {
            os << '0';
        }
        else {
            uint_t base = 10;
            const char* table = lower;
            if (os.flags () & std::ios::hex) base = 16;
            if (os.flags () & std::ios::oct) base = 8;
            if (os.flags () & std::ios::uppercase) table = upper;
            auto i = buff_size;

            div_t temp;
            temp.quot = x;
            temp.rem = 0;

            if (num_of_bits < 4 + (base == 16)) {
                // No need to loop.
                buff[-- i] = table[temp.quot.least_significant_block()];
            }
            else
            while (temp.quot > 0) {
                assert (i != 0);
                temp = divMod(temp.quot, base);
                buff[-- i] = table[temp.rem.least_significant_block()];
            }

            if (os.flags () & std::ios::showpos) {
                assert (i != 0);
                buff[-- i] = '+';
            }

            try {
                os.write (&buff[i], buff_size - i);
            }
            catch (...) {
                if (dynamicBuffer) {
                    delete [] buff;
                }

                throw;
            }
        }

        if (dynamicBuffer) {
            delete [] buff;
        }

        return os;
    }

    // only for demoting primitives
    template <typename T>
    typename std::enable_if<(sizeof(T)*8u <= bits_in_block), T>::type
    demote () const {
        return least_significant_block();
    }

    // only for demoting primitives
    template <typename T>
    typename std::enable_if<(sizeof(T)*8u > bits_in_block), T>::type
    demote () const {
        T out = 0;
        // TODO: can be optimized
        for (std::size_t i = num_of_blocks; i --> 0u; ) {
            out <<= bits_in_block;
            out += block(i);
        }

        return out;
    }

    inline void dump_blocks (std::ostream& os) const {
        for (std::size_t i = 0u; i < num_of_blocks; ++i)
            os << std::hex << static_cast<uint64_t>(block(i)) << ' ';
        os << std::endl;
    }

    template <unsigned N1, typename B1>
    typename std::enable_if<(N1 <= bits_in_block), uint_t<N1, B1> >::type
    demote_to_other_uint_t () const {
        return least_significant_block();
    }

    template <unsigned N1, typename B1>
    typename std::enable_if<(N1 > bits_in_block), uint_t<N1, B1> >::type
    demote_to_other_uint_t () const {
        uint_t<N1, B1> out = 0;
        // TODO: can be optimized
        for (std::size_t i = num_of_blocks; i --> 0u; ) {
            out <<= bits_in_block;
            out += block(i);
        }

        return out;
    }

    inline void clear_unused_bits () { clear_unused_bits_ (); }

private: /* Methods: */


    /**************************
     * Implementation details *
     **************************/


    inline block_t& least_significant_block () { return *begin(); }
    inline block_t& most_significant_block () { return *(end() - 1); }
    inline const block_t& least_significant_block () const { return *begin(); }
    inline const block_t& most_significant_block () const { return *(end() - 1); }

    inline void shift_blocks_right_(std::size_t const offset = 1u) {
        assert(offset <= num_of_blocks);
        for (std::size_t i = offset; i < num_of_blocks; ++i)
            block(i - offset) = block(i);
        for (std::size_t i = num_of_blocks - offset; i < num_of_blocks; ++i)
            block(i) = 0;
    }

    inline void shift_blocks_left_(std::size_t offset = 1u) {
        assert(offset <= num_of_blocks);
        for (std::size_t i = num_of_blocks; i --> offset; )
            block(i) = block(i - offset);
        for (std::size_t i = 0u; i < offset; ++i)
            block(i) = 0;
        clear_unused_bits_();
    }

    inline void clear_unused_bits_() {
        most_significant_block() &= last_block_mask;
    }

    inline void increment_ (block_t carry) {
        for (std::size_t i = 0u; i < num_of_blocks; ++i) {
            block(i) += carry;
            carry = block(i) < carry;
        }

        clear_unused_bits_();
    }

    inline void decrement_ (block_t borrow) {
        for (std::size_t i = 0u; i < num_of_blocks; ++i) {
            const block_t temp = block(i);
            block(i) -= borrow;
            borrow = borrow > temp;
        }

        clear_unused_bits_();
    }

    // TODO: we are assuming that blocks are not greater than 64 bits!
    inline uint64_t as_uint64_t () const {
        uint64_t out = 0;
        std::size_t const s = min(num_of_bits, 64u);
        for (std::size_t i = 0u, j = 0u; j < s; ++i, j += bits_in_block)
            out ^= static_cast<uint64_t>(block (i)) << j;
        return out;
    }


    template <typename T, class Enabled = void>
    struct copy_bits_ { };

    template <typename T>
    struct copy_bits_<T,
       typename std::enable_if<
            std::is_integral<T>::value && (sizeof (T) <= sizeof (block_t))
        >::type
    >
    {
        static void impl (uint_t<N, B>& dest, const T& val) {
            dest.least_significant_block() = static_cast<block_t>(val);
        }
    };

    template <typename T>
    struct copy_bits_<T,
        typename std::enable_if<
            std::is_integral<T>::value &&
            (sizeof (T) > sizeof (typename uint_t<N, B>::block_t))
        >::type
    >
    {
        static void impl (uint_t<N, B>& dest, const T& val) {
            assert (bits_in_block < sizeof(T)*8);
            T temp = val;
            for (std::size_t i = uint_t<N, B>::num_of_blocks; i --> 0u; ) {
                dest.block(i) = temp;
                temp >>= uint_t<N, B>::bits_in_block;
            }

            std::reverse(dest.begin(), dest.end());
        }
    };

    template <unsigned N2, typename B2>
    struct copy_bits_<uint_t<N2, B2>, void> {
        static void impl(uint_t<N, B>& dest, const uint_t<N2, B2>& other) {
            std::size_t const s1 =
                    uint_t<N, B>::num_of_blocks
                    * sizeof(typename uint_t<N, B>::block_t);
            std::size_t const s2 = uint_t<N2, B2>::num_of_blocks*sizeof(B2);
            std::size_t const n = std::min(s1, s2);
            std::fill(dest.begin(), dest.end(), uint_t<N, B>::block_t(0));
            std::memcpy(dest.begin (), other.begin(), n);
        }
    };

    // sigh... we have this hack because you can't fully specialize
    // a member template without having fully specialized the host.
    // So we just make pretend that we haven't fully specialized...
    template <typename T>
    struct copy_bits_<T,
        typename std::enable_if<
            std::is_same<T, uint128_t>::value
        >::type
    >
    {
        static void impl(uint_t<N, B>& dest, const T& other) {
            dest = 0;
            dest += other.upper ();
            dest <<= 64;
            dest += other.lower ();
        }
    };

    // Check if the argument type is any uint_t.
    template <typename T>
    struct is_some_uint : public std::false_type { };

    template <unsigned N2, typename B2>
    struct is_some_uint<uint_t<N2, B2> > : public std::true_type { };

    // Remove reference and constant and volatile qualifiers.
    template <typename T>
    struct remove_cv_ref : public std::remove_cv<typename std::remove_reference<T>::type> { };

    /*
     * This is a hack in order to detect things that look like random number
     * generators.  Anything that is not an integral, an uint128_t or any
     * uint_t is a random number generator. Duck typing... eughhhh...
     */
    template <typename Generator>
    struct is_random_generator : public std::integral_constant<bool,
        ! std::is_integral<typename remove_cv_ref<Generator>::type>::value &&
        ! std::is_same<typename remove_cv_ref<Generator>::type, uint128_t>::value &&
        ! is_some_uint<typename remove_cv_ref<Generator>::type>::value
    > { };

    /*
     * The following is required because we can not properly specialize (to my
     * knowledge) implicit constructors.
     */
    void constructor_ (const uint_t& other) {
        std::copy(other.begin(), other.end(), begin());
    }

    void constructor_ (uint_t&& other) {
        std::copy(other.begin(), other.end(), begin());
    }

    template <typename Generator>
    void constructor_ (Generator && generator, typename std::enable_if<is_random_generator<Generator>::value>::type* = nullptr) {
        generator(begin(), end());
        clear_unused_bits_ ();
    }

    template <typename T>
    void constructor_ (const T& val, typename std::enable_if<! is_random_generator<T>::value>::type* = nullptr) {
        std::fill(begin(), end(), block_t(0));
        copy_bits_<T>::impl(*this, val);
        clear_unused_bits_ ();
    }
};

namespace impl {

template <unsigned N, typename B>
struct demote_integer_impl<bool, uint_t<N, B>, void> {
    static inline bool demote (const uint_t<N, B> x) {
        return !!x;
    }
};

// Convert  unsigned integers to smaller uint_t
template <unsigned N, typename B, typename T>
struct demote_integer_impl<uint_t<N, B>, T,
        typename std::enable_if<
            std::is_unsigned<T>::value && (N < (sizeof (T) * 8u))
        >::type
    >
{
    static inline const uint_t<N, B> demote (T x) { return x; }
};

// Convert uint_t to smaller unsigned integers other than bool
template <unsigned N, typename B, typename T>
struct demote_integer_impl<T, uint_t<N, B>,
        typename std::enable_if<
            std::is_unsigned<T>::value &&
            ! std::is_same<T, bool>::value &&
            ((sizeof (T) * 8u) < N)
        >::type
    >
{
    static inline T demote (const uint_t<N, B> x) {
        return x.template demote<T>();
    }
};

// Convert uint_t to a smaller uint_t
template <unsigned N1, typename B1, unsigned N2, typename B2>
struct demote_integer_impl<uint_t<N1, B1>, uint_t<N2, B2>,
        typename std::enable_if<(N1 < N2)>::type
    >
{
    static inline uint_t<N1, B1> demote (const uint_t<N2, B2> x) {
        return x.template demote_to_other_uint_t<N1, B1>();
    }
};

// Convert uint_t to uint128_t if uint_t has more than 128 bits
template <unsigned N, typename B>
struct demote_integer_impl<uint128_t, uint_t<N, B>,
        typename std::enable_if<(128u < N)>::type
    >
{
    static inline uint128_t demote (const uint_t<N, B> x) {
        return x.template demote<uint128_t>();
    }
};

} /* namespace impl { */

} /* namespace sharemind { */

#endif /* SHAREMIND_UINT_H */
