#ifndef  OBJECTSTATE_H
#define OBJECTSTATE_H

#include "memory/MemoryObject.h"

class ObjectState {
public:
	ObjectState(std::shared_ptr<MemoryObject> memObjet);
private:
	uint32_t m_copyOnWriteOwner;
	std::shared_ptr<MemoryObject> m_memory;
};

#endif // ! ObjectState_j
