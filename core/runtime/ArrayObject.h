#ifndef ARRAY_OBJECT_H
#define ARRAY_OBJECT_H

#include <string>
class ArrayObject {
public:
	std::string getName() const { return m_name; }
private:
	std::string m_name;

};

#endif // !ARRAY_OBJECT_H
