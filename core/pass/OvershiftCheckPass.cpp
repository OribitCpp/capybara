#include "OvershiftCheckPass.h"
using namespace llvm;

char OvershiftCheckPass::ID;

OvershiftCheckPass::OvershiftCheckPass() :ModulePass(ID)
{
}

OvershiftCheckPass::~OvershiftCheckPass()
{
}

bool OvershiftCheckPass::runOnModule(llvm::Module& M)
{
	return false;
}

