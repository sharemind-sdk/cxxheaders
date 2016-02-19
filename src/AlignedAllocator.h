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

#ifndef SHAREMIND_ALIGNEDALLOCATOR_H
#define SHAREMIND_ALIGNEDALLOCATOR_H

#include <cstdlib>
#include <new>


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

} /* namespace sharemind { */

#endif /* SHAREMIND_ALIGNEDALLOCATOR_H */
