#ifndef  UPDATENODE_H
#define UPDATENODE_H

#include <memory>
#include <cstdint>
#include "Expr/Expr.h"
#include "runtime/ArrayObject.h"

class UpdateNode {

    // cache instead of recalc
    uint32_t m_hashValue;

public:
    const std::shared_ptr<UpdateNode> next;
    std::shared_ptr<Expr> index;
    std::shared_ptr<Expr> value;

private:
    /// size of this update sequence, including this update
    uint32_t m_size;

public:
    UpdateNode(const std::shared_ptr<UpdateNode>& _next, const std::shared_ptr<Expr>& _index,  const std::shared_ptr<Expr>& _value);

    uint32_t getSize() const { return m_size; }

    int32_t compare(const UpdateNode& b) const;
    uint32_t hash() const { return m_hashValue; }

    UpdateNode() = delete;
    ~UpdateNode() = default;

    uint32_t computeHash();
};


class UpdateList {
    friend class ReadExpr; // for default constructor

public:
    const std::shared_ptr<ArrayObject> root;

    /// pointer to the most recent update node
    std::shared_ptr<UpdateNode> head;

public:
    UpdateList(const std::shared_ptr<ArrayObject> _root, const std::shared_ptr<UpdateNode>& _head);
    UpdateList(const UpdateList& b) = default;
    ~UpdateList() = default;

    UpdateList& operator=(const UpdateList& b) = default;

    /// size of this update list
    unsigned getSize() const { return head ? head->getSize() : 0; }

    void extend(const std::shared_ptr<Expr>& index, const std::shared_ptr<Expr>& value);

    int compare(const UpdateList& b) const;
    unsigned hash() const;
};

#endif // ! UpdateNode_h
