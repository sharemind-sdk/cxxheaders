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

#ifndef SHAREMIND_SHAREDFROMTHIS_H
#define SHAREMIND_SHAREDFROMTHIS_H

#include <memory>


namespace sharemind {

/**
  Similar to std::enable_shared_from_this<T>, but also provides aliasing forms
  weakFromThis(U *) and sharedFromThis(U *), as well as weakFromThis(), which is
  not available in C++14 and earlier.
*/
template <typename T>
class EnableSharedFromThis: public std::enable_shared_from_this<T> {

public: /* Methods: */

    std::shared_ptr<T> sharedFromThis() {
        static_assert(!noexcept(this->shared_from_this()), "");
        return this->shared_from_this();
    }

    std::shared_ptr<T const> sharedFromThis() const {
        static_assert(!noexcept(this->shared_from_this()), "");
        return this->shared_from_this();
    }

    template <typename U>
    std::shared_ptr<U> sharedFromThis(U * const ptr) {
        static_assert(!noexcept(sharedFromThis()), "");
        return std::shared_ptr<U>(sharedFromThis(), ptr);
    }

    template <typename U>
    std::shared_ptr<U> sharedFromThis(U * const ptr) const {
        static_assert(!noexcept(sharedFromThis()), "");
        return std::shared_ptr<U>(sharedFromThis(), ptr);
    }

    std::weak_ptr<T> weakFromThis() noexcept {
        #if __cplusplus > 201402
        static_assert(noexcept(this->weak_from_this()), "");
        return this->weak_from_this();
        #else
        try {
            return sharedFromThis();
        } catch (std::bad_weak_ptr const &) {
            return std::weak_ptr<T>();
        }
        #endif
    }

    std::weak_ptr<T const> weakFromThis() const noexcept {
        #if __cplusplus > 201402
        static_assert(noexcept(this->weak_from_this()), "");
        return this->weak_from_this();
        #else
        try {
            return sharedFromThis();
        } catch (std::bad_weak_ptr const &) {
            return std::weak_ptr<T>();
        }
        #endif
    }

    template <typename U>
    std::weak_ptr<U> weakFromThis(U * const ptr) noexcept {
        try {
            return std::shared_ptr<U>(sharedFromThis(), ptr);
        } catch (std::bad_weak_ptr const &) {
            return std::weak_ptr<U>();
        }
    }

    template <typename U>
    std::weak_ptr<U> weakFromThis(U * const ptr) const noexcept {
        try {
            return std::shared_ptr<U>(sharedFromThis(), ptr);
        } catch (std::bad_weak_ptr const &) {
            return std::weak_ptr<U>();
        }
    }

};

} /* namespace Sharemind { */

#endif /* SHAREMIND_SHAREDFROMTHIS_H */
