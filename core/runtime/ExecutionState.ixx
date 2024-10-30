export module ExecutionState;
import <memory>;
import <vector>;
import FunctionWrapper;
import InstructionWrapper;
import StackFrame;


export class ExecutionState  {
public:
	ExecutionState& operator=(const ExecutionState&) = delete;
	ExecutionState(ExecutionState&&) = delete;
	ExecutionState& operator=(ExecutionState&&) = delete;
	ExecutionState(std::shared_ptr<FunctionWrapper> &func):
	PC(func->instructions.begin()),
	prevPC(PC)
	{
		pushFrame(func->instructions.end(), func);
		setID();

	}

	void pushFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator pc, std::shared_ptr<FunctionWrapper> func)
	{
		m_stack.emplace_back(pc, func);
	}

	void popFrame()
	{
		m_stack.pop_back();
	}




public:
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator PC;
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator prevPC;
	
private:
	void setID()
	{
		ID = ++s_total;
	}
private:
	std::vector<StackFrame> m_stack;
	static unsigned int s_total;
	unsigned int ID;
};

unsigned int ExecutionState::s_total = 0;
