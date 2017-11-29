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

#ifndef SHAREMIND_OBJECTTREE_H
#define SHAREMIND_OBJECTTREE_H

#include <functional>
#include <cassert>
#include <set>


namespace sharemind {

namespace ObjectTree {

template <class P, class C, class MutexT> class ParentNode;
template <class ParentNodeType, ParentNodeType (ParentNodeType::ParentType::*parentMember)> class ChildNode;

template <class ParentNodeType>
class ChildNodeBase {

    friend class ParentNode<typename ParentNodeType::ParentType,
                            typename ParentNodeType::ChildType,
                            typename ParentNodeType::MutexType>;

    template <class ParentNodeType_, ParentNodeType_ (ParentNodeType_::ParentType::*parentMember)>
    friend class ChildNode;

public: /* Methods: */

    virtual inline ~ChildNodeBase() noexcept {}

    inline typename ParentNodeType::ParentType * parent() const noexcept
    { return m_parent; }

private: /* Methods: */

    inline ChildNodeBase(typename ParentNodeType::ParentType * const parent,
                         typename ParentNodeType::ChildType * const realChild,
                         ParentNodeType (ParentNodeType::ParentType::*parentMember))
        : m_parent(parent)
        , m_realChild((assert(realChild), realChild))
    {
        if (parent)
            (parent->*parentMember).registerChild(this);
    }

    inline typename ParentNodeType::ChildType * realChild() const noexcept
    { return m_realChild; }

    inline void unregisterChild(
            ParentNodeType (ParentNodeType::ParentType::*parentMember)) noexcept
    {
        if (m_parent) {
            (m_parent->*parentMember).unregisterChild(this);
            m_parent = nullptr;
        }
    }

    inline typename ParentNodeType::ChildType * freeByParent() noexcept {
        m_parent = nullptr;
        return m_realChild;
    }


private: /* Fields: */

    typename ParentNodeType::ParentType * m_parent;
    typename ParentNodeType::ChildType * const m_realChild;

};

template <class ParentNodeType, ParentNodeType (ParentNodeType::ParentType::*parentMember)>
class ChildNode: public ChildNodeBase<ParentNodeType> {

public: /* Methods: */

    inline ChildNode(typename ParentNodeType::ParentType * parent,
                     typename ParentNodeType::ChildType * realChild)
        : ChildNodeBase<ParentNodeType>(parent, realChild, parentMember) {}

    inline ~ChildNode() noexcept override
    { this->unregisterChild(parentMember); }

};

template <class ParentT, class ChildT, class MutexT>
class ParentNode {

private: /* Types: */

    using SelfType = ParentNode<ParentT, ChildT, MutexT>;
    using ChildNodeType = ChildNodeBase<SelfType>;
    friend class ChildNodeBase<SelfType>;

public: /* Types: */

    using ChildType = ChildT;
    using ParentType = ParentT;
    using MutexType = MutexT;

private: /* Types: */

    struct lock_guard {

    /* Methods: */

        explicit lock_guard(MutexT & mutex) noexcept
            : m_mutex(mutex)
        { mutex.lock(); }

        ~lock_guard() noexcept { m_mutex.unlock(); }

    /* Fields: */

        MutexT & m_mutex;

    };

    using ChildNodes = std::set<ChildNodeType *>;

public: /* Methods: */

    template <typename ... Args>
    inline ParentNode(Args && ... args)
        : m_childDestructor(std::forward<Args>(args)...) {}

    inline ~ParentNode() noexcept {
        lock_guard lock(m_mutex);
        for (ChildNodeType * const node : m_childNodes)
            m_childDestructor(node->freeByParent());
        /* m_childNodes.clear(); */
    }

    inline std::set<ChildType *> children() const {
        lock_guard lock(m_mutex);
        return childrenNoLock();
    }

    inline typename ChildNodes::size_type numChildren() const noexcept {
        lock_guard lock(m_mutex);
        return m_childNodes.size();
    }

private: /* Methods: */

    inline void registerChild(ChildNodeType * const childNode) {
        lock_guard lock(m_mutex);
        assert(!m_childNodes.count(childNode));
        assert(!childrenNoLock().count(childNode->realChild()));
        m_childNodes.insert(childNode);
    }

    inline void unregisterChild(ChildNodeType * const childNode) noexcept {
        lock_guard lock(m_mutex);
        assert(m_childNodes.count(childNode));
        m_childNodes.erase(childNode);
        assert(!childrenNoLock().count(childNode->realChild()));
    }

    inline std::set<ChildType *> childrenNoLock() const {
        std::set<ChildType *> cs;
        for (ChildNodeType const * const childNode : m_childNodes)
            cs.insert(childNode->realChild());
        return cs;
    }

private: /* Fields: */

    mutable MutexT m_mutex;
    ChildNodes m_childNodes;

    const std::function<void (ChildType * child)> m_childDestructor;

};

} /* namespace ObjectTree { */
} /* namespace sharemind { */


#if 0

/* Here's an example of how to use the object tree (try to run with valgrind): */

#include <mutex>
#include <iostream>

using namespace sharemind;
using namespace std;

struct Child;

struct Parent {
    inline Parent() : parentNode(&Parent::freeChild) {}
    static void freeChild(Child * child);

    using ParentNodeType = ObjectTree::ParentNode<Parent, Child, std::mutex>;
    ParentNodeType parentNode;
};

struct Child {

    ObjectTree::ChildNode<Parent::ParentNodeType, &Parent::parentNode>
            childNode;

    Child(Parent * parent, int v) : childNode(parent, this), m_a(v)
        { cout << "Child(): " << m_a << endl;}

    ~Child() noexcept { cout << "~Child(): " << m_a << endl; }

    int m_a;

};

void Parent::freeChild(Child * child) noexcept { delete child; }

int main() {
    Parent * p = new Parent();
    Child * c1 = new Child(p, 1);
    new Child(p, 2);
    delete c1;
    delete p;
    return 0;
}

#endif

#endif /* SHAREMIND_OBJECTTREE_H */
