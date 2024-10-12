#ifndef  DIVCHECKPASS_H
#define DIVCHECKPASS_H

#include <llvm/Pass.h>

class DivCheckPass : public llvm::ModulePass {
	static char ID;
public:
	DivCheckPass();
	~DivCheckPass();
	bool runOnModule(llvm::Module& M) override;
};

#endif // ! DIVCHECKPASS_H

