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

#ifndef SHAREMIND_BITVECTOR_H
#define SHAREMIND_BITVECTOR_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <vector>


namespace sharemind {

/*
 * This is very close to boost::dynamic_bitset. For example, the reference to an individual
 * bit is expressed exactly the same way. However, there are some major modifications:
 * 1) we do not have use for many of the more complicated features of dynamic_bitset
 * 2) we need to be able to supply linearly growing underlying container for the block vector
 * 3) we need to be able to access the underlying memory for efficient randomization and serialization
 * 4) we do not keep the invariant that excess bits are zeroed
 * 5) behaviour is closer to std::vector than boost::dynamic_bitset
 *
 * It's possible that the Block size need not be polymorphic, this would simplify (de)serialization greatly.
 */

template < typename Block = std::uint64_t
         , typename Alloc = std::allocator<Block>
         , typename BlockStore = std::vector<Block, Alloc>
         >
class __attribute__ ((visibility("internal"))) BitVec {
private: /* Types: */

    using allocator_type = Alloc;
    using block_type = Block;
    using store_type = BlockStore;

    static constexpr std::size_t bytes_per_block()
    { return sizeof(block_type); }

    static constexpr std::size_t bits_per_block()
    { return 8u * bytes_per_block(); }

    /* The following is to reduce the use of magic constant 64.
     * We assume that the serialization block width is bigger than the bit vectors block width.
     */
    struct Serialization {
        using block_type = std::uint64_t;
        static constexpr std::size_t block_width()
        { return 8u * sizeof(block_type); }
    };

    static_assert(sizeof(block_type) <= sizeof(typename Serialization::block_type),
                  "Serialization block must be at least as big as the integers block.");
    static_assert(Serialization::block_width() % bits_per_block() == 0,
                  "Serialization block size must be divisible by the number of bits in the integers block.");

public:

    using size_type = typename store_type::size_type;
    using value_type = bool;
    using const_reference = bool;

    class reference {
        friend class BitVec<Block, Alloc, BlockStore>;
    private: /* Methods: */

        reference (block_type& b, block_type pos)
            : m_block (b)
            , m_mask (block_type (1) << pos)
        { }

        void operator& (); // DO NOT IMPLEMENT

    public:

        operator bool () const { return (m_block & m_mask) != 0; }
        bool operator ~ () const { return (m_block & m_mask) == 0; }

        reference& flip() { flip_bit_ (); return *this; }

        reference& operator = (bool x)               { assign_bit_ (x);   return *this; }
        reference& operator = (const reference& rhs) { assign_bit_ (rhs); return *this; }

        /*
         * if statements turned out to be more efficient than bit hackery
         */

        reference& operator |= (bool x) { if  (x) set_bit_ ();   return *this; }
        reference& operator &= (bool x) { if (!x) reset_bit_ (); return *this; }
        reference& operator ^= (bool x) { if  (x) flip_bit_ ();  return *this; }
        reference& operator -= (bool x) { if  (x) reset_bit_ (); return *this; }

    private:

        inline void set_bit_ () const { m_block |= m_mask; }
        inline void reset_bit_ () const { m_block &= ~m_mask; }
        inline void flip_bit_ () const { m_block ^= m_mask; }
        inline void assign_bit_ (bool x) const { x ? set_bit_ () : reset_bit_ (); }

    private: /* Fields: */
        block_type&       m_block;
        const block_type  m_mask;
    };

public: /* Methods: */

    explicit
    BitVec (const Alloc& alloc = Alloc ())
        : m_blocks (alloc)
        , m_num_bits (0)
    { }

    explicit
    BitVec (size_type num_bits, bool val = false, const Alloc& alloc = Alloc ())
        : m_blocks (compute_block_count (num_bits), val ? ~ block_type (0) : block_type (0), alloc)
        , m_num_bits (num_bits)
    { }

    BitVec(BitVec&& other)
        : m_blocks{std::move(other.m_blocks)}
        , m_num_bits{std::move(other.m_num_bits)}
    {
        other.m_num_bits = 0;
    }

    BitVec& operator=(BitVec&& other) {
        m_blocks = std::move(other.m_blocks);
        m_num_bits = other.m_num_bits;
        other.m_num_bits = 0;
        return *this;
    }

    template <typename InputIterator>
    BitVec (InputIterator begin, InputIterator end, const Alloc& alloc = Alloc ())
        : m_blocks (alloc)
        , m_num_bits (std::distance (begin, end))
    {
        const size_type num_blocks = compute_block_count (m_num_bits);
        m_blocks.resize (num_blocks);
        assign (begin, end);
    }

    ~BitVec () {
        m_num_bits = 0;
    }

    void clear () {
        m_blocks.clear ();
        m_num_bits = 0;
    }

    const_reference operator [] (size_type idx) const {
        assert (idx < m_num_bits);
        return (m_blocks[block_index (idx)] & bit_mask (idx)) != 0;
    }

    /** \throws std::out_of_range */
    const_reference at (size_type idx) const {
        if (idx >= m_num_bits)
            throw std::out_of_range ("sharemind::BitVec::at");
        return (*this)[idx];
    }

    reference operator [] (size_type idx) {
        assert (idx < m_num_bits);
        return reference (m_blocks[block_index (idx)], bit_index (idx));
    }

    /** \throws std::out_of_range */
    reference at (size_type idx) {
        if (idx >= m_num_bits)
            throw std::out_of_range ("sharemind::BitVec::at");
        return (*this)[idx];
    }

    size_type size () const {
        return m_num_bits;
    }

    bool empty () const {
        return m_num_bits == 0;
    }

    const_reference front () const {
        assert (! empty ());
        return (*this)[0];
    }

    const_reference back () const {
        assert (! empty ());
        return (*this)[m_num_bits - 1];
    }

    /** \todo it's probably worth to set the undefined valued bits */
    void resize (size_type num_bits) {
        m_blocks.resize (compute_block_count (num_bits));
        m_num_bits = num_bits;
    }

    void swap (BitVec& other) {
        m_blocks.swap (other.m_blocks);
        std::swap (m_num_bits, other.m_num_bits);
    }

    void push_back (bool val) {
        if (m_num_bits % bits_per_block() == 0) {
            m_blocks.push_back (0);
        }

        block_type& block = m_blocks.back ();
        const block_type mask = bit_mask (m_num_bits);
        block = val ? (block | mask) : (block & ~ mask);
        ++ m_num_bits;
    }

    void pop_back () {
        assert (! empty ());
        -- m_num_bits;
        if (m_num_bits % bits_per_block() == 0) {
            m_blocks.pop_back ();
        }
    }

    void assign (const BitVec& other) {
        assert (size () == other.size ());
        std::copy (other.m_blocks.begin (), other.m_blocks.end (), m_blocks.begin ());
    }

    void assign (const size_type & n, const value_type & val) {
        resize(n);
        val ? set() : reset();
    }

    template <typename InputIterator >
    void assign (InputIterator begin, InputIterator end) {
        if (begin != end) {
            typename std::iterator_traits<InputIterator>::iterator_category category;
            assign_ (begin, end, category);
        }
    }

    template <typename InputIterator>
    void assignBits (InputIterator begin, InputIterator end) {
        using input_value_type = typename std::iterator_traits<InputIterator>::value_type;
        const std::vector<input_value_type> temp (begin, end);
        const std::size_t size = temp.size ();
        assert (this->size () == (8 * sizeof (input_value_type) * size));
        if (! m_blocks.empty ()) {
            std::memcpy(&m_blocks[0], &temp[0], sizeof (input_value_type) * size);
        }
    }

    void append (const BitVec& other) {
        using const_iter = typename store_type::const_iterator;
        block_type r = m_num_bits % bits_per_block();
        const_iter i = other.m_blocks.begin ();
        const const_iter e = other.m_blocks.end ();
        if (r == 0) {
            for (; i != e; ++ i) {
                m_blocks.push_back (*i);
            }
        }
        else {
            assert (! empty ());
            assert (! m_blocks.empty ());
            m_blocks.back () |= (*i << r);
            do {
                const block_type b = *i >> (bits_per_block() - r);
                ++ i;
                m_blocks.push_back (b | (i == e ? 0 : *i << r));
            } while (i != e);
        }

        m_num_bits += other.m_num_bits;
    }

    template <typename Rng>
    void randomize (Rng &rng) {
        if (! empty ()) {
            rng.fillBytes (&m_blocks[0], sizeof (block_type) * num_blocks_ ());
        }
    }

    template <typename InMessage> bool deserialize(InMessage &msg);
    template <typename OutMessage> void serialize(OutMessage &msg) const;

    allocator_type get_allocator () const {
        return m_blocks.get_allocator ();
    }

    BitVec& set () {
        std::fill (m_blocks.begin (), m_blocks.end (), ~ block_type (0));
        return *this;
    }

    BitVec& reset () {
        std::fill (m_blocks.begin (), m_blocks.end (), block_type (0));
        return *this;
    }

    BitVec& flip () {
        using iter = typename store_type::iterator;
        for (iter i = m_blocks.begin (), e = m_blocks.end (); i != e; ++ i) {
            *i = ~ *i;
        }

        return *this;
    }

    BitVec& operator ^= (const BitVec& other) {
        assert (size () == other.size ());
        for (size_type i = 0; i < num_blocks_ (); ++ i) {
            m_blocks[i] ^= other.m_blocks[i];
        }

        return *this;
    }

    BitVec& operator |= (const BitVec& other) {
        assert (size () == other.size ());
        for (size_type i = 0; i < num_blocks_ (); ++ i) {
            m_blocks[i] |= other.m_blocks[i];
        }

        return *this;
    }

    BitVec& operator &= (const BitVec& other) {
        assert (size () == other.size ());
        for (size_type i = 0; i < num_blocks_ (); ++ i) {
            m_blocks[i] &= other.m_blocks[i];
        }

        return *this;
    }

    template <typename B, typename A, typename S>
    friend bool operator == (const BitVec<B, A, S>& x,
                             const BitVec<B, A, S>& y);

    inline void clear_and_release () {
        store_type().swap (m_blocks);
    }

    void* data () { return static_cast<void*>(m_blocks.data ()); }
    const void* data () const { return static_cast<const void*>(m_blocks.data ()); }

private: /* Methods: */

    static size_type block_index (size_type pos) { return pos / bits_per_block(); }
    static block_type bit_index (size_type pos) { return static_cast<block_type>(pos % bits_per_block()); }
    static block_type bit_mask (size_type pos) { return block_type (1) << bit_index (pos); }
    static size_type compute_block_count (size_type num_bits) {
        return (num_bits / bits_per_block()) + static_cast<size_type>(num_bits % bits_per_block() != 0);
    }

    static size_type serialization_block_count (size_type num_bits) {
        return (num_bits / Serialization::block_width())
               + static_cast<size_type>(num_bits % Serialization::block_width() != 0);
    }


    inline size_type num_blocks_ () const { return m_blocks.size (); }

    template <typename InputIterator>
    void assign_ (InputIterator begin, InputIterator end, std::input_iterator_tag) {
        std::vector<typename std::iterator_traits<InputIterator>::value_type > temp (begin, end);
        assign_ (temp.begin (), temp.end (), std::random_access_iterator_tag ());
    }

    template <typename InputIterator>
    void assign_ (InputIterator begin, InputIterator end, std::forward_iterator_tag) {
        assert(size() == static_cast<typename std::make_unsigned<typename std::iterator_traits<InputIterator>::difference_type>::type>(std::distance(begin, end)));
        reset ();
        typename store_type::iterator block_iter = m_blocks.begin ();
        block_type mask = 1;
        for (InputIterator i = begin; i != end; ++ i) {
            if (*i) { *block_iter |= mask; }
            if ((mask <<= 1) == 0) {
                ++ block_iter;
                mask = 1;
            }
        }
    }

    bool equals_ (const BitVec& other) const {
        if (m_num_bits != other.m_num_bits)
            return false;

        if (! empty ()) {
            std::size_t const n = other.m_blocks.size ();
            std::size_t i = 0;
            for (i = 0; i < n - 1; ++ i) {
                if (m_blocks[i] != other.m_blocks[i])
                    return false;
            }

            block_type mask = bit_mask (m_num_bits - 1); /* find the highest bit */
            mask |= (mask - 1); /* set all lower bits (including the highest) */
            if (((m_blocks[i] ^ other.m_blocks[i]) & mask) != 0) {
                return false;
            }
        }

        return true;
    }

private: /* Fields: */
    store_type   m_blocks;
    size_type    m_num_bits;
};

template <typename B, typename A, typename S>
BitVec<B, A, S> operator & (const BitVec<B, A, S>& x, const BitVec<B, A, S>& y) {
    BitVec<B, A, S> result (x);
    return (result &= y);
}

template <typename B, typename A, typename S>
BitVec<B, A, S> operator ^ (const BitVec<B, A, S>& x, const BitVec<B, A, S>& y) {
    BitVec<B, A, S> result (x);
    return (result ^= y);
}

template <typename B, typename A, typename S>
BitVec<B, A, S> operator | (const BitVec<B, A, S>& x, const BitVec<B, A, S>& y) {
    BitVec<B, A, S> result (x);
    return (result |= y);
}

template <typename B, typename A, typename S>
bool operator == (const BitVec<B, A, S>& x, const BitVec<B, A, S>& y) {
    return x.equals_ (y);
}

template <typename B, typename A, typename S>
bool operator != (const BitVec<B, A, S>& x, const BitVec<B, A, S>& y) {
    return !(x == y);
}

template <typename B, typename A, typename S>
template <typename InMessage>
bool BitVec<B, A, S>::deserialize (InMessage &msg) {
    typename Serialization::block_type num_bits = 0;
    if (! msg.read (num_bits)) {
        return false;
    }

    /* overflow check: */
    if (num_bits > std::numeric_limits<size_type>::max ()) {
        return false;
    }

    if (num_bits != m_num_bits) {
        return false;
    }

    reset ();
    size_type block_index = 0;
    const size_type msg_blocks = serialization_block_count (num_bits);
    for (size_type i = 0; i < msg_blocks; ++ i) {
        typename Serialization::block_type block = 0;
        if (! msg.read (block))
            return false;

        for (size_type b = 0; b < Serialization::block_width(); b += bits_per_block()) {
            if (block_index == num_blocks_ ())
                break;

            m_blocks[block_index] |= block_type (block >> b);
            ++ block_index;
        }
    }

    assert (block_index == num_blocks_ ());
    return true;
}

template <typename B, typename A, typename S>
template <typename OutMessage>
void BitVec<B, A, S>::serialize (OutMessage & msg) const {
    const typename Serialization::block_type num_bits = m_num_bits;
    msg.write(num_bits);
    size_type block_index = 0;
    const size_type msg_blocks = serialization_block_count (num_bits);
    for (size_type i = 0; i < msg_blocks; ++ i) {
        typename Serialization::block_type block = 0;
        for (size_type b = 0; b < Serialization::block_width(); b += bits_per_block()) {
            if (block_index == num_blocks_ ())
                break;

            block |= typename Serialization::block_type (m_blocks[block_index]) << b;
            ++ block_index;
        }

        msg.write(block);
    }
    assert (block_index == num_blocks_ ());
}

} /* namespace sharemind */

#endif /* SHAREMIND_BITVECTOR_H */
