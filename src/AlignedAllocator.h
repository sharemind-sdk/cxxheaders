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

#ifndef SHAREMIND_ALIGNEDALLOCATOR_H
#define SHAREMIND_ALIGNEDALLOCATOR_H

#include <cstdlib>
/* #include <limits> */
#include <new>
#include <memory>
/* #include <type_traits> */
/* #include <utility> */


namespace sharemind {

template <std::size_t ALIGNMENT>
inline void * allocateAlignedStorage(std::size_t const size,
                                     std::nothrow_t const &) noexcept
{
    void * p;
    if (::posix_memalign(&p, ALIGNMENT, size) != 0)
        return nullptr;
    return p;
}

template <std::size_t ALIGNMENT>
inline void * allocateAlignedStorage(std::size_t const size) {
    if (void * const p = allocateAlignedStorage<ALIGNMENT>(size, std::nothrow))
        return p;
    throw std::bad_alloc();
}

inline void freeAlignedStorage(void * const ptr) noexcept { std::free(ptr); }

#define SHAREMIND_ALIGNEDALLOCATION_MEMBERS(alignment) \
    void * operator new(std::size_t size) \
    { return ::sharemind::template allocateAlignedStorage<alignment>(size); } \
    void * operator new[](std::size_t size) \
    { return ::sharemind::template allocateAlignedStorage<alignment>(size); } \
    void * operator new(std::size_t size, std::nothrow_t const & nt) noexcept \
    { return ::sharemind::template allocateAlignedStorage<alignment>(size, nt); }\
    void * operator new[](std::size_t size, std::nothrow_t const & nt) noexcept\
    { return ::sharemind::template allocateAlignedStorage<alignment>(size, nt); }\
    void operator delete(void * p) \
    { ::sharemind::freeAlignedStorage(p); } \
    void operator delete[](void * p) \
    { ::sharemind::freeAlignedStorage(p); } \
    void operator delete(void * p, std::nothrow_t const &) \
    { ::sharemind::freeAlignedStorage(p); } \
    void operator delete[](void * p, std::nothrow_t const &) \
    { ::sharemind::freeAlignedStorage(p); }

template <typename T>
struct AlignedAllocator {

    /* The commented-out members are optional. To access their default
       implementations, use template std::allocator_traits on this class. */

/* Types: */

    using type = AlignedAllocator<T>;

    using pointer = T *;

    /*
    using const_pointer =
            typename std::pointer_traits<pointer>::template rebind<T const>;
    using void_pointer =
            typename std::pointer_traits<pointer>::template rebind<void>;
    using const_void_pointer =
            typename std::pointer_traits<pointer>::template rebind<void const>;
    */

    using value_type = T;
    using difference_type =
            typename std::pointer_traits<pointer>::difference_type;
    using size_type =
            typename std::make_unsigned<difference_type>::type;

    /*
    template <typename U> struct rebind { using other = AlignedAllocator<U>; };

    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;
    */

/* Methods: */

    AlignedAllocator() noexcept {}
    AlignedAllocator(type &&) noexcept {}
    AlignedAllocator(type const &) noexcept {}

    template <typename U>
    AlignedAllocator(AlignedAllocator<U> &&) noexcept {}

    template <typename U>
    AlignedAllocator(AlignedAllocator<U> const &) noexcept {}

    static pointer allocate(size_type const n) {
        return static_cast<pointer>(
                    allocateAlignedStorage<alignof(T)>(n * sizeof(T)));
    }

    /*
    static pointer allocate(size_type const n, const_pointer const = nullptr)
    { return allocate(n); }
    */

    static void deallocate(pointer const p, size_type const) noexcept
    { freeAlignedStorage(p); }

    /*
    constexpr static size_type max_size() noexcept
    { return std::numeric_limits<size_type>::max(); }

    template <typename U, typename ... Args>
    static void construct(U * const p, Args && ... args)
    { ::new(static_cast<void *>(p)) U(std::forward<Args>(args)...); }

    template <typename U> static void destroy(U * const p) { p->~U(); }

    static type select_on_container_copy_construction() noexcept
    { return type(); }
    */

};

template <typename T1, typename T2>
constexpr bool operator==(AlignedAllocator<T1> const &,
                          AlignedAllocator<T2> const &) noexcept
{ return true; }

template <typename T1, typename T2>
constexpr bool operator!=(AlignedAllocator<T1> const &,
                          AlignedAllocator<T2> const &) noexcept
{ return false; }

} /* namespace sharemind { */

#endif /* SHAREMIND_ALIGNEDALLOCATOR_H */
