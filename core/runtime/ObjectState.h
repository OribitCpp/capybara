#ifndef  OBJECTSTATE_H
#define OBJECTSTATE_H

#include "memory/MemoryObject.h"
#include "runtime/ArrayObject.h"
#include <vector>

class ObjectState {
	friend class AddressSpace;
public:
	ObjectState(std::shared_ptr<MemoryObject> memObjet);
	ObjectState(std::shared_ptr<MemoryObject> memObjet, const std::shared_ptr<ArrayObject> &arrayObj);
	void setReadOnly(bool value) { m_readOnly = value; }
	bool getReadOnly() const { return m_readOnly; }
	uint64_t getSize() const { return m_size; }
	std::shared_ptr<MemoryObject> getMemoryObject() const { return m_memory; }
	void initializeToZero();
	void initializeToRandom();


	std::shared_ptr<Expr> read(std::shared_ptr<Expr> offset, uint32_t width) const;
	std::shared_ptr<Expr> read(uint32_t offset, uint32_t width) const;
	std::shared_ptr<Expr> read8(uint32_t offset) const;

	void write(uint32_t offset, std::shared_ptr<Expr> value);
	void write(std::shared_ptr<Expr> offset, std::shared_ptr<Expr> value);

	void write8(uint32_t offset, uint8_t value);
	void write16(uint32_t offset, uint16_t value);
	void write32(uint32_t offset, uint32_t value);
	void write64(uint32_t offset, uint64_t value);

private:
	void makeConcrete();

	bool isByteConcrete(unsigned offset) const;

	/// isByteKnownSymbolic ==> !isByteConcrete
	bool isByteKnownSymbolic(unsigned offset) const;

	/// isByteUnflushed(i) => (isByteConcrete(i) || isByteKnownSymbolic(i))
	bool isByteUnflushed(unsigned offset) const;
	void markByteConcrete(unsigned offset);
	void markByteSymbolic(unsigned offset);
	void markByteFlushed(unsigned offset);
	void markByteUnflushed(unsigned offset);
	void setKnownSymbolic(unsigned offset, std::shared_ptr<Expr> value);

	void write8(uint64_t offset, std::shared_ptr<Expr> value);
	void write8(std::shared_ptr<Expr> offset, std::shared_ptr<Expr> value);
private:
	bool m_readOnly;
	uint32_t m_copyOnWriteOwner;
	uint64_t m_size;
	std::shared_ptr<MemoryObject> m_memory;
	std::vector<uint8_t> m_concreteStore;
	std::vector<bool> m_concreteMask;
	std::vector<bool> m_unflushedMask;
	std::vector<std::shared_ptr<Expr>> m_knownSymbolics;
};

std::ostream& operator<<(std::ostream& os, ObjectState& object);

#endif // ! OBJECTSTATE_H
