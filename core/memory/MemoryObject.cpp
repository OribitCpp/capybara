#include "MemoryObject.h"
#include "MemoryManager.h"

uint32_t MemoryObject::counter = 0;

MemoryObject::~MemoryObject()
{
	MemoryManager::instance().markFree(this);
}
