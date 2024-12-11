#ifndef ARRAYEXPRHASH_H
#define ARRAYEXPRHASH_H

#include "runtime/ArrayObject.h"
#include "UpdateNode.h"

struct ArrayHashFn {
	unsigned operator()(const std::shared_ptr<ArrayObject> &array) const {
		return(array ? array->hash() : 0);
	}
};

struct ArrayCmpFn {
	bool operator()(const std::shared_ptr<ArrayObject>& array1, const std::shared_ptr<ArrayObject>&array2) const {
		return(array1 == array2);
	}
};

struct UpdateNodeHashFn {
	unsigned operator()(const std::shared_ptr<UpdateNode>&un) const {
		return(un ? un->hash() : 0);
	}
};

struct UpdateNodeCmpFn {
	bool operator()(const std::shared_ptr<UpdateNode>&un1, const std::shared_ptr<UpdateNode>& un2) const {
		return(un1 == un2);
	}
};

template<class T>
class ArrayExprHash {
public:

	ArrayExprHash() {};
	// Note: Extend the class and overload the destructor if the objects of type T
	// that are to be hashed need to be explicitly destroyed
	// As an example, see class STPArrayExprHash
	virtual ~ArrayExprHash() {};

	bool lookupArrayExpr(const std::shared_ptr<ArrayObject>& array, T& exp) const;
	void hashArrayExpr(const std::shared_ptr<ArrayObject>& array, T& exp);

	bool lookupUpdateNodeExpr(const std::shared_ptr<UpdateNode>& un, T& exp) const;
	void hashUpdateNodeExpr(const std::shared_ptr<UpdateNode>& un, T& exp);

protected:
	std::unordered_map<const std::shared_ptr<ArrayObject>, T, ArrayHashFn, ArrayCmpFn> m_array_hash;
	std::unordered_map<const std::shared_ptr<UpdateNode>, T, UpdateNodeHashFn, UpdateNodeCmpFn> m_update_node_hash;
};



template<class T>
inline bool ArrayExprHash<T>::lookupArrayExpr(const std::shared_ptr<ArrayObject>& array, T& exp) const
{
	bool res = false;
	assert(array);
	typename std::unordered_map<const std::shared_ptr<ArrayObject>, T, ArrayHashFn, ArrayCmpFn>::const_iterator it = m_array_hash.find(array);
	if (it != m_array_hash.end()) {
		exp = it->second;
		res = true;
	}
	return res;
}

template<class T>
inline void ArrayExprHash<T>::hashArrayExpr(const std::shared_ptr<ArrayObject>& array, T& exp)
{
	assert(array);
	m_array_hash[array] = exp;
}

template<class T>
inline bool ArrayExprHash<T>::lookupUpdateNodeExpr(const std::shared_ptr<UpdateNode>& un, T& exp) const
{
	bool res = false;
	assert(un);
	typename std::unordered_map<const std::shared_ptr<UpdateNode>, T, UpdateNodeHashFn, UpdateNodeCmpFn>::const_iterator it = m_update_node_hash.find(un);
	if (it != m_update_node_hash.end()) {
		exp = it->second;
		res = true;
	}
	return res;
}

template<class T>
inline void ArrayExprHash<T>::hashUpdateNodeExpr(const std::shared_ptr<UpdateNode>& un, T& exp)
{
	assert(un);
	m_update_node_hash[un] = exp;
}





#endif