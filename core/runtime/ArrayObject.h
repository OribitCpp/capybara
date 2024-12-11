#ifndef ARRAY_OBJECT_H
#define ARRAY_OBJECT_H


#include <cstdint>
#include <string>
#include <memory>

#include "expr/ConstantExpr.h"

class ArrayObject {
public:
	std::string getName() const { return m_name; }
	uint32_t domain() const { return m_domain; }
	uint32_t range() const { return m_range; }
	const std::vector<std::shared_ptr<ConstantExpr>>& getValues() const { return m_constantValues; }
	uint32_t hash() const { return m_hashValue; }
private:
	std::vector<std::shared_ptr<ConstantExpr>> m_constantValues;
	std::string m_name;
	uint32_t m_domain;
	uint32_t m_range;
	uint32_t m_hashValue;
};

struct ArrayObjectHash {
	bool operator()(const std::shared_ptr<ArrayObject> &object) const {
		return object->hash();
	}
};

#endif // !ARRAY_OBJECT_H
