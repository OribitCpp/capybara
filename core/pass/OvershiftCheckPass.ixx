export module OvershiftCheckPass;
import <llvm/Pass.h>;

export class OvershiftCheckPass : public llvm::ModulePass {
	static char ID;
public:
	OvershiftCheckPass() :ModulePass(ID)
	{
	}
	~OvershiftCheckPass()
	{

	}

	bool runOnModule(llvm::Module& M) override
	{
		return false;
	}

};

