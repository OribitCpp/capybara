export module DivCheckPass;

import <llvm/Pass.h>;

export class DivCheckPass : public llvm::ModulePass {
	static char ID;
public:
	DivCheckPass():ModulePass(ID)
	{

	}
	~DivCheckPass();
	bool runOnModule(llvm::Module& M) override
	{
		return false;
	}
};


char DivCheckPass::ID;
