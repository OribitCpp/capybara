#include "ObjectState.h"
#include "expr/ReadExpr.h"
#include "expr/Cast/ZExtExpr.h"
#include "expr/ExtractExpr.h"

ObjectState::ObjectState(std::shared_ptr<MemoryObject> memObjet):
	m_memory(memObjet), 
	m_copyOnWriteOwner(0),
	m_readOnly(false),
	m_size(memObjet->size)
{
	m_concreteStore.resize(m_size, 0);
}

ObjectState::ObjectState(std::shared_ptr<MemoryObject> memObjet, const std::shared_ptr<ArrayObject>& arrayObj):
	m_memory(memObjet),
	m_copyOnWriteOwner(0),
	m_readOnly(false),
	m_size(memObjet->size)
{
	m_concreteStore.resize(m_size, 0);
}

void ObjectState::initializeToZero()
{
	makeConcrete();
	for (size_t i = 0; i < m_concreteStore.size(); i++)
	{
		m_concreteStore[i] = 0;
	}
}

void ObjectState::initializeToRandom()
{
	makeConcrete();
	for (size_t i = 0; i < m_concreteStore.size(); i++)
	{
		m_concreteStore[i] = 0xAB;
	}
}

void ObjectState::makeConcrete()
{
	m_concreteMask.resize(0);
	m_knownSymbolics.resize(0);
	m_unflushedMask.resize(0);
}

bool ObjectState::isByteConcrete(unsigned offset) const
{
	return m_concreteMask.empty() || m_concreteMask[offset];
}

bool ObjectState::isByteKnownSymbolic(unsigned offset) const
{
	return !m_knownSymbolics.empty() && m_knownSymbolics[offset];
}

bool ObjectState::isByteUnflushed(unsigned offset) const
{
	return m_unflushedMask.empty() || m_unflushedMask[offset];
}

void ObjectState::markByteConcrete(unsigned offset)
{
	m_concreteMask[offset]= true;
}

void ObjectState::markByteSymbolic(unsigned offset)
{
	if (m_concreteMask.empty()) {
		m_concreteMask.resize(m_size, true);
	}
	m_concreteMask[offset] = false;
}

void ObjectState::markByteFlushed(unsigned offset)
{
	if (m_unflushedMask.empty()) {
		m_unflushedMask.resize(m_size, false);
	}
	else {
		m_unflushedMask[offset] = true;
	}
}

void ObjectState::markByteUnflushed(unsigned offset)
{
	if (m_unflushedMask.empty() == false)
		m_unflushedMask[offset] = true;
}

void ObjectState::setKnownSymbolic(unsigned offset, std::shared_ptr<Expr> value)
{
	if (m_knownSymbolics.empty() == false) {
		m_knownSymbolics[offset] = value;
	}
	else {
		if (value) {
			m_knownSymbolics.resize(m_size);
			m_knownSymbolics[offset] = value;
		}
	}
}

void ObjectState::write8(uint64_t offset, std::shared_ptr<Expr> value)
{
	if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(value)) {
		write8(offset, (uint8_t)CE->getZExtValue(8));
	}
	else {
		setKnownSymbolic(offset, value);

		markByteSymbolic(offset);
		markByteUnflushed(offset);
	}
}

std::shared_ptr<Expr> ObjectState::read(std::shared_ptr<Expr> offset, uint32_t width) const
{
	return std::shared_ptr<Expr>();
}

std::shared_ptr<Expr> ObjectState::read8(uint32_t offset) const
{
	if (isByteConcrete(offset)) {
		return std::make_shared<ConstantExpr>(m_concreteStore[offset], 8);
	}
	else if (isByteKnownSymbolic(offset)) {
		return m_knownSymbolics[offset];
	}
	else {
		assert(!isByteUnflushed(offset) && "unflushed byte without cache value");

		//return std::make_shared<ReadExpr>(getUpdates(), std::make_shared<ConstantExpr>(32, offset));
		return nullptr;
	}
}

void ObjectState::write(uint32_t offset, std::shared_ptr<Expr> value)
{
	if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(value)) {
		uint32_t w = CE->getWidth();
		if (w <= 64 && isPowerOfTwo(w)) {
			uint64_t val = CE->getZExtValue();
			switch (w) {
			default: assert(0 && "Invalid write size!");
			case  1:
			case  8:  write8(offset, val); return;
			case 16: write16(offset, val); return;
			case 32: write32(offset, val); return;
			case 64: write64(offset, val); return;
			}
		}
	}

	// Treat bool specially, it is the only non-byte sized write we allow.
	uint32_t w = value->getWidth();
	if (w == 1) {
		write8(offset, std::make_shared<ZExtExpr>(value, 8));
		return;
	}

	// Otherwise, follow the slow general case.
	unsigned NumBytes = w / 8;
	assert(w == NumBytes * 8 && "Invalid write size!");
	for (uint64_t i = 0; i != NumBytes; ++i) {
		//unsigned idx = Context::get().isLittleEndian() ? i : (NumBytes - i - 1);
		write8(offset + i, std::make_shared<ExtractExpr>(value, 8 * i, 8));
	}
}

void ObjectState::write(std::shared_ptr<Expr> offset, std::shared_ptr<Expr> value)
{
}

void ObjectState::write8(uint32_t offset, uint8_t value)
{
	m_concreteStore[offset] = value;
	setKnownSymbolic(offset, 0);

	markByteConcrete(offset);
	markByteUnflushed(offset);
}

void ObjectState::write16(uint32_t offset, uint16_t value)
{
}

void ObjectState::write32(uint32_t offset, uint32_t value)
{
}

void ObjectState::write64(uint32_t offset, uint64_t value)
{
}

std::ostream& operator<<(std::ostream& os, ObjectState& object)
{
	return os;
}
