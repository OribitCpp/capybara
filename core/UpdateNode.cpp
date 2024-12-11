#include "UpdateNode.h"
#include <assert.h>

UpdateNode::UpdateNode(const std::shared_ptr<UpdateNode>& _next, const std::shared_ptr<Expr>& _index, const std::shared_ptr<Expr>& _value)
    : next(_next), index(_index), value(_value) {
    // FIXME: What we need to check here instead is that _value is of the same width 
    // as the range of the array that the update node is part of.
    /*
    assert(_value->getWidth() == Expr::Int8 &&
           "Update value should be 8-bit wide.");
    */
    computeHash();
    m_size = next ? next->m_size + 1 : 1;

}

int32_t UpdateNode::compare(const UpdateNode& b) const
{
    //if (int i = index.compare(b.index))
    //    return i;
    //return value.compare(b.value);
    return 0;
}

uint32_t UpdateNode::computeHash()
{
    m_hashValue = index->hash() ^ value->hash();
    if (next)
        m_hashValue ^= next->hash();
    return m_hashValue;
}

UpdateList::UpdateList(const std::shared_ptr<ArrayObject> _root, const std::shared_ptr<UpdateNode>& _head)
    : root(_root), head(_head) {}

void UpdateList::extend(const std::shared_ptr<Expr>& index, const std::shared_ptr<Expr>& value)
{

    if (root) {
        assert(root->domain() == index->getWidth());
        assert(root->range() == value->getWidth());
    }

    head = std::make_shared<UpdateNode>(head, index, value);
}

int UpdateList::compare(const UpdateList& b) const
{
    return 0;
}

unsigned UpdateList::hash() const
{
    return 0;
}
