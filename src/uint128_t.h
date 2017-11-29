/*
 * Copyright (C) Cybernetica
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

#ifndef SHAREMIND_UINT128_H
#define SHAREMIND_UINT128_H

#include "integer_demotion.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <type_traits>


namespace sharemind {

/**
 * Unsigned 128-bit integers.
 *
 * Implicit conversions to other integer types are not supported.  If one
 * wishes to support both way implicit conversions then one needs to template
 * all the operations. Implicit to-boolean conversion is not allowed for the
 * same reason. Frankly, C++11 would be really helpful with explicit conversion
 * operators!
 *
 * There are a lot of optimization opportunities (division).
 */
class uint128_t {
public: /* Types: */

    class division_by_zero_error : public std::runtime_error {
    public: /* Methods: */
        division_by_zero_error ()
            : std::runtime_error ("Division by zero")
        { }
    };

public: /* Methods: */

    uint128_t () { }

    template <typename T>
    uint128_t (T rhs)
        : m_upper (0)
        , m_lower (static_cast<uint64_t>(rhs))
    { }

    uint128_t (uint64_t upper, uint64_t lower)
        : m_upper (upper)
        , m_lower (lower)
    { }

    template <typename T>
    uint128_t& operator = (T rhs) {
        m_upper = 0;
        m_lower = static_cast<uint64_t>(rhs);
        return *this;
    }

    uint64_t upper () const { return m_upper; }
    uint64_t lower () const { return m_lower; }
    bool operator ! () const { return ! static_cast<bool>(m_upper | m_lower); }
    uint128_t operator ~ () const { return uint128_t (~ m_upper, ~ m_lower); }
    uint128_t& operator ++ () { *this += 1; return *this; }
    uint128_t operator ++ (int) { const uint128_t t = *this; ++ *this; return t; }
    uint128_t& operator -- () { *this -= 1; return *this; }
    uint128_t operator -- (int) { const uint128_t t = *this; -- *this; return t; }
    uint128_t& operator &= (uint128_t x) { *this = *this & x; return *this; }
    uint128_t& operator ^= (uint128_t x) { *this = *this ^ x; return *this; }
    uint128_t& operator |= (uint128_t x) { *this = *this | x; return *this; }
    uint128_t& operator += (uint128_t x) { *this = *this + x; return *this; }
    uint128_t& operator -= (uint128_t x) { *this = *this - x; return *this; }
    uint128_t& operator *= (uint128_t x) { *this = *this * x; return *this; }
    uint128_t& operator /= (uint128_t x) { *this = *this / x; return *this; }
    uint128_t& operator %= (uint128_t x) { *this = *this % x; return *this; }
    uint128_t& operator >>= (uint64_t shift) { *this = *this >> shift; return *this; }
    uint128_t& operator <<= (uint64_t shift) { *this = *this << shift; return *this; }

    static uint128_t random ();

private: /* Methods: */

    friend uint128_t operator & (uint128_t x, uint128_t y);
    friend uint128_t operator ^ (uint128_t x, uint128_t y);
    friend uint128_t operator | (uint128_t x, uint128_t y);
    friend uint128_t operator - (uint128_t x);
    friend uint128_t operator + (uint128_t x, uint128_t y);
    friend uint128_t operator - (uint128_t x, uint128_t y);
    friend uint128_t operator * (uint128_t x, uint128_t y);
    friend uint128_t operator / (uint128_t x, uint128_t y);
    friend uint128_t operator % (uint128_t x, uint128_t y);
    friend uint128_t operator >> (uint128_t x, uint64_t shift);
    friend uint128_t operator << (uint128_t x, uint64_t shift);

    friend bool operator == (uint128_t x, uint128_t y);
    friend bool operator != (uint128_t x, uint128_t y);
    friend bool operator <  (uint128_t x, uint128_t y);
    friend bool operator <= (uint128_t x, uint128_t y);
    friend bool operator >  (uint128_t x, uint128_t y);
    friend bool operator >= (uint128_t x, uint128_t y);

    friend std::ostream& operator << (std::ostream& os, uint128_t x);

    static void divMod (uint128_t x, uint128_t y, uint128_t& quotient, uint128_t& reminder);

private: /* Fields: */
    uint64_t m_upper;
    uint64_t m_lower;
};

/*
 * Bitwise operations:
 */

inline uint128_t operator & (uint128_t x, uint128_t y) { return uint128_t (x.m_upper & y.m_upper, x.m_lower & y.m_lower); }

inline uint128_t operator ^ (uint128_t x, uint128_t y) { return uint128_t (x.m_upper ^ y.m_upper, x.m_lower ^ y.m_lower); }

inline uint128_t operator | (uint128_t x, uint128_t y) { return uint128_t (x.m_upper | y.m_upper, x.m_lower | y.m_lower); }

inline uint128_t operator << (uint128_t x, uint64_t shift) {
    if (shift == 0) return x;
    assert (shift < 128 && "Left shift on uint128_t of 128 bits or more.");
    if (shift < 64)
        return uint128_t ((x.m_upper << shift) | x.m_lower >> (64 - shift), x.m_lower << shift);
    else
        return uint128_t (x.m_lower << (shift - 64), 0);
}

inline uint128_t operator >> (uint128_t x, uint64_t shift) {
    if (shift == 0) return x;
    assert (shift < 128 && "Right shift on uint128_t of 128 bits or more.");
    if (shift < 64)
        return uint128_t (x.m_upper >> shift, x.m_lower >> shift | (x.m_upper << (64 - shift)));
    else
        return uint128_t (0, x.m_upper >> (shift - 64));
}

/*
 * Arithmetic operations:
 */

inline uint128_t operator - (uint128_t x) {
    return uint128_t (- x.m_upper - static_cast<uint64_t>(x.m_lower > 0), - x.m_lower);
}

inline uint128_t operator + (uint128_t x, uint128_t y) {
    const uint64_t upper = x.m_upper + y.m_upper;
    const uint64_t lower = x.m_lower + y.m_lower;
    const uint64_t overflow = static_cast<uint64_t>(lower < x.m_lower);
    return uint128_t (upper + overflow, lower);
}

inline uint128_t operator - (uint128_t x, uint128_t y) {
    const uint64_t upper = x.m_upper - y.m_upper;
    const uint64_t lower = x.m_lower - y.m_lower;
    const uint64_t overflow = static_cast<uint64_t>(lower > x.m_lower);
    return uint128_t (upper - overflow, lower);
}

/**
 * 1. split both numbers to 4 32 bit blocks
 * 2. multiply relevant blocks together (some would obviously overflow)
 * 3. add blocks that are under the same exponent together while being careful with overflow
 * 4. add previous sums together with correct exponents
 */
inline uint128_t operator * (uint128_t x, uint128_t y) {
    const uint64_t MASK = 0xffffffffULL;
    const uint64_t xs[4] = { x.m_upper >> 32, x.m_upper & MASK, x.m_lower >> 32, x.m_lower & MASK };
    const uint64_t ys[4] = { y.m_upper >> 32, y.m_upper & MASK, y.m_lower >> 32, y.m_lower & MASK };
    const uint64_t muls[10] = { xs[0] * ys[3], xs[1] * ys[2], xs[1] * ys[3], xs[2] * ys[1], xs[2] * ys[2],
                                xs[2] * ys[3], xs[3] * ys[0], xs[3] * ys[1], xs[3] * ys[2], xs[3] * ys[3] };

    uint64_t vals[4] = {0, 0, 0, 0};
    vals[0] += (muls[9] & MASK);
    vals[1] += (muls[5] & MASK) + (muls[9] >> 32);
    vals[2] += (muls[2] & MASK) + (muls[5] >> 32);
    vals[3] += (muls[0] & MASK) + (muls[2] >> 32);
    vals[1] += (muls[8] & MASK);
    vals[2] += (muls[4] & MASK) + (muls[8] >> 32);
    vals[3] += (muls[1] & MASK) + (muls[4] >> 32);
    vals[2] += (muls[7] & MASK);
    vals[3] += (muls[3] & MASK) + (muls[7] >> 32);
    vals[3] += (muls[6] & MASK);

    return uint128_t (vals[0]) + uint128_t (vals[1] >> 32, vals[1] << 32) + uint128_t (vals[2], 0) + uint128_t (vals[3] << 32, 0);
}

/**
 * Standard bit-wise long division (see wikipedia). Would be much more
 * efficient to use the same approach as multiplication and try to divide using
 * bigger chunks.
 */
inline void uint128_t::divMod (uint128_t x, uint128_t y, uint128_t& quotient, uint128_t& reminder) {
    if (y == 0) {
        throw division_by_zero_error ();
    }

    quotient = 0;
    reminder = 0;

    uint128_t bit (uint64_t (1) << 63, 0);
    while (bit != 0) {
        reminder <<= 1;
        reminder.m_lower |= !!(x & bit);
        if (reminder >= y) {
            reminder -= y;
            quotient |= bit;
        }

        bit >>= 1;
    }
}

inline uint128_t operator / (uint128_t x, uint128_t y) {
    uint128_t d, r;
    uint128_t::divMod (x, y, d, r);
    return d;
}

inline uint128_t operator % (uint128_t x, uint128_t y) {
    uint128_t d, r;
    uint128_t::divMod (x, y, d, r);
    return r;
}

/*
 * Comparison operations:
 */

inline bool operator == (uint128_t x, uint128_t y) { return x.m_upper == y.m_upper && x.m_lower == y.m_lower; }
inline bool operator != (uint128_t x, uint128_t y) { return !(x == y); }
inline bool operator < (uint128_t x, uint128_t y) { return x.m_upper < y.m_upper || (x.m_upper == y.m_upper && x.m_lower < y.m_lower); }
inline bool operator > (uint128_t x, uint128_t y) { return y < x; }
inline bool operator <= (uint128_t x, uint128_t y) { return !(x > y); }
inline bool operator >= (uint128_t x, uint128_t y) { return !(x < y); }

/*
 * Output:
 */

inline std::ostream& operator << (std::ostream& os, uint128_t x) {
    // enough to fit the number in base 8 + extra:
    constexpr std::size_t buff_size = 64;
    static char const lower[] = "0123456789abcdef";
    static char const upper[] = "0123456789ABCDEF";
    char buff[buff_size];

    if (x == 0) os << '0';
    if (x > 0) {
        unsigned base = 10;
        const char* table = lower;
        if (os.flags () & std::ios::hex) base = 16;
        if (os.flags () & std::ios::oct) base = 8;
        if (os.flags () & std::ios::uppercase) table = upper;
        uint128_t r;
        auto i = buff_size;
        while (x > 0) {
            uint128_t::divMod (x, base, x, r);
            buff[-- i] = table[r.lower()];
        }

        if (os.flags () & std::ios::showpos)
            buff[-- i] = '+';

        static_assert(buff_size
                      <= static_cast<std::make_unsigned<std::streamsize>::type>(
                                std::numeric_limits<std::streamsize>::max()),
                      "");
        os.write (&buff[i], static_cast<std::streamsize>(buff_size - i));
    }

    return os;
}

namespace impl {

template <>
struct demote_integer_impl<bool, uint128_t, void> {
    static inline bool demote (const uint128_t x) {
        return demote_integer<bool>(x.upper () | x.lower ());
    }
};

template <>
struct demote_integer_impl<uint64_t, uint128_t, void> {
    static inline uint64_t demote (const uint128_t x) {
        return x.lower ();
    }
};

// Demote to unsigned integers other than bool and uint64_t
template <typename T>
struct demote_integer_impl<T, uint128_t,
        typename std::enable_if<
            std::is_unsigned<T>::value &&
            ! std::is_same<T, bool>::value &&
            ! std::is_same<T, uint64_t>::value
        >::type
    >
{
    static inline T demote (const uint128_t x) {
        return demote_integer<T>(x.lower ());
    }
};

} /* namespace impl { */

} /* namespace sharemind { */

#endif /* SHAREMIND_UINT128_H */
