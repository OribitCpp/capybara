#ifndef OVERSHIFTCHECKPASS_H
#define OVERSHIFTCHECKPASS_H

import <llvm/Pass.h>

class OvershiftCheckPass : public llvm::ModulePass {
	static char ID;
public:
	OvershiftCheckPass();
	~OvershiftCheckPass();

	bool runOnModule(llvm::Module& M) override;

};

#endif // !OVERSHIFTCHECKPASS_H


