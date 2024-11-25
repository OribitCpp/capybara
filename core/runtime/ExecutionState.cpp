#include "ExecutionState.h"
#include "memory/MemoryManager.h"
#include "InstructionWrapper.h"
#include <sstream>
#include <iomanip>

 uint64_t ExecutionState::s_total = 0;

ExecutionState::ExecutionState(std::shared_ptr<FunctionWrapper>& func):
	PC(func->instructions.begin()),
	prevPC(PC)
{

	pushFrame(func->instructions.end(), func);
	setID();
}

void ExecutionState::pushFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator pc, std::shared_ptr<FunctionWrapper> func)
{
    std::shared_ptr<StackFrame> stackFrame = std::make_shared<StackFrame>(pc, func);
	m_stack.emplace_back(stackFrame);
}

void ExecutionState::popFrame()
{
	m_stack.pop_back();
}

void ExecutionState::saveInAllocator(const std::shared_ptr<MemoryObject>& object, bool isLocal)
{
	if (isLocal) {
		m_heapAllocator.insert(object);
	}
	else {
		m_stackAllocator.insert(object);
	}
}

void ExecutionState::remove(const std::shared_ptr<MemoryObject>& object)
{
	m_stackAllocator.erase(object);
	m_heapAllocator.erase(object);
}

size_t ExecutionState::getStackSize() const
{
	return m_stack.size();
}

void ExecutionState::bindLocal(const std::shared_ptr<InstructionWrapper> wrapper,const std::shared_ptr<Expr>& expr)
{
	m_stack.back()->locals[wrapper->dest] = expr;
}

std::shared_ptr<Expr> ExecutionState::eval(const std::shared_ptr<InstructionWrapper>& wrapper, uint32_t index)
{
	uint32_t number = wrapper->instruction->getNumOperands();
	return m_stack.back()->locals[number];
}

void ExecutionState::dumpStack(llvm::raw_ostream& out) const
{
    unsigned idx = 0;
    std::vector<std::shared_ptr<InstructionWrapper>>::iterator target = prevPC;
    for (std::vector<std::shared_ptr<StackFrame>>::const_reverse_iterator  it = m_stack.rbegin(); it != m_stack.rend(); ++it) {
        const std::shared_ptr<StackFrame> sf = *it;
        llvm::Function* f = sf->function->function;
        const std::shared_ptr<InstructionWrapper> &ii = *target;
        out << "\t#" << idx++;
        std::stringstream AssStream;
        AssStream << std::setw(8) << std::setfill('0') << ii->assemblyLine;
        out << AssStream.str();
        out << " in " << f->getName().str() << "(";
        // Yawn, we could go up and print varargs if we wanted to.
        unsigned index = 0;
        for (llvm::Function::arg_iterator ai = f->arg_begin(), ae = f->arg_end(); ai != ae; ++ai) {
            if (ai != f->arg_begin()) out << ", ";

            if (ai->hasName())  out << ai->getName().str() << "=";

            std::shared_ptr<Expr> value = sf->locals[sf->function->getArgRegister(index++)];
            if (std::dynamic_pointer_cast<ConstantExpr>(value)) {
                out << value;
            }
            else {
                out << "symbolic";
            }
        }
        out << ")";
        if (false == ii->file.empty())
            out << " at " << ii->file << ":" << ii->line;
        out << "\n";
        target = sf->caller;
    }
}

std::shared_ptr<Expr> ExecutionState::getArgumentExpr(const std::shared_ptr<FunctionWrapper>& func, uint32_t index)
{
    return m_stack.back()->locals[func->getArgRegister(index)];
}

void ExecutionState::bindArgument(const std::shared_ptr<FunctionWrapper>& func, uint32_t index, const std::shared_ptr<Expr>& expr)
{
    m_stack.back()->locals[func->getArgRegister(index)] = expr;
}

std::shared_ptr<StackFrame> ExecutionState::getStackFrame()
{
	return m_stack.back();
}

void ExecutionState::setID()
{
	m_id = ++s_total;
}
