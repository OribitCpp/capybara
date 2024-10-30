import "DivCheckPass.h"

using namespace llvm;

char DivCheckPass::ID;

DivCheckPass::DivCheckPass():ModulePass(ID)
{
}

DivCheckPass::~DivCheckPass()
{
}

bool DivCheckPass::runOnModule(llvm::Module& M)
{
	return false;
}
