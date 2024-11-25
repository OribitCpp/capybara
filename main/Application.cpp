#include "Application.h"

#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GetElementPtrTypeIterator.h>
#include <llvm/IR/IntrinsicInst.h>

#include "expr/SelectExpr.h"
#include "expr/ExtractExpr.h"
#include "expr/ConcatExpr.h"
#include "expr/Arithmetic/SDivExpr.h"
#include "expr/Arithmetic/AddExpr.h"
#include "expr/Arithmetic/MulExpr.h"
#include "expr/Arithmetic/SRemExpr.h"
#include "expr/Arithmetic/SubExpr.h"
#include "expr/Arithmetic/UDivExpr.h"
#include "expr/Arithmetic/URemExpr.h"
#include "expr/Cast/SExtExpr.h"
#include "expr/Cast/ZExtExpr.h"
#include "expr/Compare/EqExpr.h"
#include "expr/Compare/NeExpr.h"
#include "expr/Compare/SgeExpr.h"
#include "expr/Compare/SgtExpr.h"
#include "expr/Compare/SleExpr.h"
#include "expr/Compare/SltExpr.h"
#include "expr/Compare/UgeExpr.h"
#include "expr/Compare/UgtExpr.h"
#include "expr/Compare/UleExpr.h"
#include "expr/Compare/UltExpr.h"
#include "expr/Bit/AndExpr.h"
#include "expr/Bit/OrExpr.h"
#include "expr/Bit/AShrExpr.h"
#include "expr/Bit/LShrExpr.h"
#include "expr/Bit/NotExpr.h"
#include "expr/Bit/ShlExpr.h"
#include "expr/Bit/XorExpr.h"

#include "runtime/ModuleWrapper.h"
#include "FileLoader.h"
#include "memory/MemoryManager.h"
#include "memory/MemoryObject.h"
#include "runtime/InstructionWrapper.h"
#include "statistic/StatisticManager.h"
#include "GlobalSetting.h"

#include <cassert>

Application::Application(int argc, char* argv[])
{
	parseConsoleParameters(argc, argv);
}

void Application::init() {
	Logger::info("Appplication Init Stage");
	llvm::InitializeNativeTarget();
	m_llvmContext = std::make_shared<llvm::LLVMContext>();
	m_externalDispatcher = std::make_shared<ExternalDispatcher>(*m_llvmContext);
}
void Application::ready() {
	std::vector<std::unique_ptr<llvm::Module>> modules;
	FileLoader fileLoader(*m_llvmContext);
	fileLoader.load("D:/capybara/get_sign.bc", modules);
	Logger::info("loaded D:/capybara/get_sign.bc");

	std::unique_ptr<llvm::Module> finalModule = ModuleWrapper::linkModules(modules);	
	m_moduleStorage = std::make_shared<ModuleWrapper>(finalModule);
	m_moduleStorage->optimiseWithPass();
}

void Application::execute()
{
	std::shared_ptr<FunctionWrapper> mainFunction = m_moduleStorage->getFunction("main");
	std::shared_ptr<ExecutionState> mainFuncState = std::make_shared<ExecutionState>(mainFunction);
	initializeGlobals(mainFuncState);
    m_statesSet.insert(mainFuncState);
    while (false == m_statesSet.empty() &&m_haltExecution == false)
    {
        std::shared_ptr<ExecutionState> executionState = *m_statesSet.begin();
        m_statesSet.erase(m_statesSet.begin());
        auto instructionIter = executionState->PC;
        executionState->prevPC = instructionIter;
        ++executionState->PC;
        executeInstructon(executionState, *instructionIter);
    }
}


void Application::parseConsoleParameters(int argc, char* argv[])
{
	// -proj-dir: the project directory
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i],"-help")==0 || strcmp(argv[i],"-h") == 0) {
		}
		else if (argv[i] == "") {

		}
	}
}

void Application::initializeGlobals(std::shared_ptr<ExecutionState>& state)
{
	allocateGlobalObjects(state);
	for (const llvm::GlobalAlias& alias : m_moduleStorage->getOriginPtr()->aliases()) {
		initializeGlobalsAllias(&alias);
	}
	initializeGlobalObjects(state);
}

void Application::allocateGlobalObjects(std::shared_ptr<ExecutionState>& state)
{
	llvm::Module* modulePtr = m_moduleStorage->getOriginPtr();
	if (modulePtr->getModuleInlineAsm().empty() == false)
		Logger::warn("ignor module level assembly");

	for (llvm::Function &func : *modulePtr)
	{
		std::shared_ptr<ConstantExpr> address;
		if (func.hasExternalWeakLinkage() && m_externalDispatcher->resolvesSymbol(func.getName().str())) {
			address = ConstantExpr::createPointer(0);
		}
		else {
			std::shared_ptr<MemoryObject> object = MemoryManager::alloc(8);
			object->isLocal = false;
			object->isGlobal = true;
			state->saveInAllocator(object, object->isLocal);
			address = ConstantExpr::createPointer(object->address);
			m_legalFunctions.emplace(object->address, &func);
		}
		m_globalAddresses.emplace(&func, address);
	}

	for (const llvm::GlobalVariable& variable : modulePtr->globals()) {
		uint64_t size = m_moduleStorage->getTypeSize(variable.getValueType());
		std::shared_ptr<MemoryObject> memObj = MemoryManager::alloc(size);
		memObj->isLocal = false;
		memObj->isGlobal = true;
		memObj->allocSite = &variable;
		m_globalObjects.emplace(&variable, memObj);
		m_globalAddresses.emplace(&variable, memObj->getBaseExpr());
	}
}

void Application::initializeGlobalsAllias(const llvm::Constant* constant)
{
	const llvm::GlobalAlias* gAlias = llvm::dyn_cast<llvm::GlobalAlias>(constant);
	if (gAlias) {
		if (m_globalAddresses.count(gAlias)) return;
		const llvm::Constant* aliasee = gAlias->getAliasee();
		if (const llvm::GlobalValue* gValue = llvm::dyn_cast<llvm::GlobalValue>(aliasee)) {
			auto iter = m_globalAddresses.find(gValue);
			if (iter != m_globalAddresses.end()) {
				m_globalAddresses.emplace(gAlias, iter->second);
				return;
			}
		}
	}
	for(const auto *operand : constant->operand_values())
	{
		initializeGlobalsAllias(llvm::cast<llvm::Constant>(operand));
	}
	if (gAlias) {
		m_globalAddresses.emplace(gAlias, evalConstant(gAlias->getAliasee()));
	}
}

void Application::initializeGlobalObjects(std::shared_ptr<ExecutionState>& state)
{
    const llvm::Module* m = m_moduleStorage->getOriginPtr();

    for (const llvm::GlobalVariable& v : m->globals()) {
        //MemoryObject* mo = m_globalObjects.find(&v)->second;
        //ObjectState* os = bindObjectInState(state, mo, false);

        //if (v.isDeclaration() && mo->size) {
        //    // Program already running -> object already initialized.
        //    // Read concrete value and write it to our copy.
        //    void* addr;
        //    if (v.getName() == "__dso_handle") {
        //        addr = &__dso_handle; // wtf ?
        //    }
        //    else {
        //        addr = externalDispatcher->resolveSymbol(v.getName().str());
        //    }
        //    if (!addr) {
        //        klee_error("Unable to load symbol(%.*s) while initializing globals",
        //            static_cast<int>(v.getName().size()), v.getName().data());
        //    }
        //    for (unsigned offset = 0; offset < mo->size; offset++) {
        //        os->write8(offset, static_cast<unsigned char*>(addr)[offset]);
        //    }
        //}
        //else if (v.hasInitializer()) {
        //    initializeGlobalObject(state, os, v.getInitializer(), 0);
        //    if (v.isConstant()) {
        //        os->setReadOnly(true);
        //        // initialise constant memory that may be used with external calls
        //        state.addressSpace.copyOutConcrete(mo, os);
        //    }
        //}
        //else {
        //    os->initializeToRandom();
        //}
    }
}

std::shared_ptr<ConstantExpr> Application::evalConstant(const llvm::Constant* c, std::shared_ptr<InstructionWrapper> instruction)
{
	if (!instruction) {
		std::shared_ptr<ConstantWrapper> constantWrapper = m_moduleStorage->getConstant(c);
		if (constantWrapper) instruction = constantWrapper->instructionWrapper;
	}

    if (const llvm::ConstantExpr *expr = llvm::dyn_cast<llvm::ConstantExpr>(c)) {
        return evalConstantExpr(expr, instruction);
    }
    else {
        if (const llvm::ConstantInt* ci = llvm::dyn_cast<llvm::ConstantInt>(c)) {
            return std::make_shared<ConstantExpr>(ci->getValue());
        }
        else if (const llvm::ConstantFP* cf = llvm::dyn_cast<llvm::ConstantFP>(c)) {
            return std::make_shared<ConstantExpr>(cf->getValueAPF().bitcastToAPInt());
        }
        else if (const llvm::GlobalValue* gv = llvm::dyn_cast<llvm::GlobalValue>(c)) {
            auto it = m_globalAddresses.find(gv);
            assert(it != m_globalAddresses.end());
            return it->second;
        }
        else if (llvm::isa<llvm::ConstantPointerNull>(c)) {
            return ConstantExpr::createPointer(0);
        }
        else if (llvm::isa<llvm::UndefValue>(c) || llvm::isa<llvm::ConstantAggregateZero>(c)) {
            if (m_moduleStorage->getTypeSizeInBits(c->getType()) == 0) {
                if (llvm::isa<llvm::LandingPadInst>(instruction->instruction)) {
                    Logger::warn("Using zero size array fix for landingpad instruction filter");
                    return std::make_shared<ConstantExpr>(1, 0);
                }
            }
            return std::make_shared<ConstantExpr>(llvm::APInt(m_moduleStorage->getTypeSizeInBits(c->getType()), 0));
        }
        else if (const llvm::ConstantDataSequential* cds = llvm::dyn_cast<llvm::ConstantDataSequential>(c)) {
            // Handle a vector or array: first element has the smallest address,
            // the last element the highest
            std::shared_ptr<ConstantExpr> result = std::make_shared<ConstantExpr>(0, 0);
            for (unsigned i = cds->getNumElements(); i != 0; --i) {
                std::shared_ptr<ConstantExpr> constantExpr = evalConstant(cds->getElementAsConstant(i - 1), instruction);
                result = result->Concat(constantExpr);
            }
            /* assert(Context::get().isLittleEndian() &&
                 "FIXME:Broken for big endian");*/
            return result;
        }
        else if (const llvm::ConstantStruct* cs = llvm::dyn_cast<llvm::ConstantStruct>(c)) {
            std::shared_ptr<ConstantExpr> result = std::make_shared<ConstantExpr>(0, 0);
            const llvm::StructLayout* sl = m_moduleStorage->getStructLayout(cs->getType());
            for (unsigned i = cs->getNumOperands(); i != 0; --i) {
                unsigned op = i - 1;
                std::shared_ptr<ConstantExpr> kid = evalConstant(cs->getOperand(op), instruction);

                uint64_t thisOffset = sl->getElementOffsetInBits(op),
                    nextOffset = (op == cs->getNumOperands() - 1)
                    ? sl->getSizeInBits()
                    : sl->getElementOffsetInBits(op + 1);
                if (nextOffset - thisOffset > kid->getWidth()) {
                    uint64_t paddingWidth = nextOffset - thisOffset - kid->getWidth();
                    result = result->Concat(std::make_shared<ConstantExpr>(llvm::APInt(paddingWidth, 0)));
                }
                result = result->Concat(kid);
            }
            //assert(Context::get().isLittleEndian() &&
            //    "FIXME:Broken for big endian");
            return result;
        }
        else if (const llvm::ConstantArray* ca = dyn_cast<llvm::ConstantArray>(c)) {
            std::shared_ptr<ConstantExpr> result = std::make_shared<ConstantExpr>(0, 0);
            for (unsigned i = ca->getNumOperands(); i != 0; --i) {
                result = result->Concat(evalConstant(ca->getOperand(i - 1), instruction));
            }
            //assert(Context::get().isLittleEndian() &&
            //    "FIXME:Broken for big endian");
            return result;
        }
        else if (const llvm::ConstantVector* cv = llvm::dyn_cast<llvm::ConstantVector>(c)) {
            std::shared_ptr<ConstantExpr> result = std::make_shared<ConstantExpr>(0, 0);
            for (unsigned i = cv->getNumOperands(); i != 0; --i) {
               result = result->Concat(evalConstant(cv->getOperand(i - 1), instruction));
            }
            //assert(Context::get().isLittleEndian() &&
            //    "FIXME:Broken for big endian");
            return result;
        }
        else if (const llvm::BlockAddress* ba = llvm::dyn_cast<llvm::BlockAddress>(c)) {
            // return the address of the specified basic block in the specified function
            const llvm::BasicBlock *arg_bb = (llvm::BasicBlock*)ba->getOperand(1);
            const auto res = ConstantExpr::createPointer(reinterpret_cast<std::uint64_t>(arg_bb));
            return std::dynamic_pointer_cast<ConstantExpr>(res);
        }
        else  Logger::error("Cannot handle constant  at location {}", instruction ? instruction->sourceLocation() : "[unknown]");
    }
    return nullptr;
}

std::shared_ptr<ConstantExpr> Application::evalConstantExpr(const llvm::ConstantExpr* expr, const std::shared_ptr<InstructionWrapper> & instruction)
{
	llvm::Type *type = expr->getType();
    uint64_t typeSize = m_moduleStorage->getTypeSize(type);
	std::shared_ptr<ConstantExpr> op1, op2, op3;
	int numOperands = expr->getNumOperands();

	if (numOperands > 0) op1 = evalConstant(expr->getOperand(0), instruction);
	if (numOperands > 1) op2 = evalConstant(expr->getOperand(1), instruction);
	if (numOperands > 2) op3 = evalConstant(expr->getOperand(2), instruction);
    switch (expr->getOpcode()) {
        case llvm::Instruction::SDiv:
        case llvm::Instruction::UDiv:
        case llvm::Instruction::SRem:
        case llvm::Instruction::URem:
            if (op2->getLimitedValue() == 0){
                Logger::error("Division/modulo by zero during constant folding at location {} ", instruction ? instruction->sourceLocation() : "[unknown]");
            }
            break;
        case llvm::Instruction::Shl:
        case llvm::Instruction::LShr:
        case llvm::Instruction::AShr:
            if (op2->getLimitedValue() >= op1->getWidth()) {
                Logger::error("Overshift during constant folding at location {} ", instruction ? instruction->sourceLocation() : "[unknown]");
            }
     }

    switch (expr->getOpcode()) {
        default:
            Logger::error("Unknown ConstantExpr type {}", instruction ? instruction->sourceLocation() : "[unknown]");

        case llvm::Instruction::Trunc:
            return op1->Extract(0, typeSize);
    case llvm::Instruction::ZExt:  return op1->ZExt(typeSize);
    case llvm::Instruction::SExt:  return op1->SExt(typeSize);
    case llvm::Instruction::Add:   return op1->Add(op2);
    case llvm::Instruction::Sub:   return op1->Sub(op2);
    case llvm::Instruction::Mul:   return op1->Mul(op2);
    case llvm::Instruction::SDiv:  return op1->SDiv(op2);
    case llvm::Instruction::UDiv:  return op1->UDiv(op2);
    case llvm::Instruction::SRem:  return op1->SRem(op2);
    case llvm::Instruction::URem:  return op1->URem(op2);
    case llvm::Instruction::And:   return op1->And(op2);
    case llvm::Instruction::Or:    return op1->Or(op2);
    case llvm::Instruction::Xor:   return op1->Xor(op2);
    case llvm::Instruction::Shl:   return op1->Shl(op2);
    case llvm::Instruction::LShr:  return op1->LShr(op2);
    case llvm::Instruction::AShr:  return op1->AShr(op2);
    case llvm::Instruction::BitCast:  return op1;

    case llvm::Instruction::IntToPtr:
        return op1->ZExt(typeSize);

    case llvm::Instruction::PtrToInt:
        return op1->ZExt(typeSize);

    case llvm::Instruction::GetElementPtr: {
        std::shared_ptr<ConstantExpr> base = op1->ZExt(sizeof(void *));
        for (llvm::gep_type_iterator ii = llvm::gep_type_begin(expr), ie = llvm::gep_type_end(expr); ii != ie; ++ii) {
            std::shared_ptr<ConstantExpr> indexOp = evalConstant(llvm::cast<llvm::Constant>(ii.getOperand()), instruction);
            if (indexOp->isZero()) continue;

            // Handle a struct index, which adds its field offset to the pointer.
            if (auto STy = ii.getStructTypeOrNull()) {
                unsigned ElementIdx = indexOp->getZExtValue();
                const llvm::StructLayout* SL = m_moduleStorage->getStructLayout(STy);
                base = base->Add(std::make_shared<ConstantExpr>(llvm::APInt(sizeof(void*), SL->getElementOffset(ElementIdx))));
                continue;
            }

            // For array or vector indices, scale the index by the size of the type.
            // Indices can be negative
            uint64_t value = m_moduleStorage->getTypeAllocSize(ii.getIndexedType());
            std::shared_ptr<ConstantExpr> tmp = std::make_shared<ConstantExpr>(llvm::APInt(sizeof(void*), value));
            base = base->Add(indexOp->SExt(sizeof(void*))->Mul(tmp));
        }
        return base;

    }

    case llvm::Instruction::ICmp: {
        switch (expr->getPredicate()) {
        default: assert(0 && "unhandled ICmp predicate");
        case llvm::ICmpInst::ICMP_EQ:  return op1->Eq(op2);
        case llvm::ICmpInst::ICMP_NE:  return op1->Ne(op2);
        case llvm::ICmpInst::ICMP_UGT: return op1->Ugt(op2);
        case llvm::ICmpInst::ICMP_UGE: return op1->Uge(op2);
        case llvm::ICmpInst::ICMP_ULT: return op1->Ult(op2);
        case llvm::ICmpInst::ICMP_ULE: return op1->Ule(op2);
        case llvm::ICmpInst::ICMP_SGT: return op1->Sgt(op2);
        case llvm::ICmpInst::ICMP_SGE: return op1->Sge(op2);
        case llvm::ICmpInst::ICMP_SLT: return op1->Slt(op2);
        case llvm::ICmpInst::ICMP_SLE: return op1->Sle(op2);
        }
    }

    case llvm::Instruction::Select:
        return op1->isTrue() ? op2 : op3;

    case llvm::Instruction::FAdd:
    case llvm::Instruction::FSub:
    case llvm::Instruction::FMul:
    case llvm::Instruction::FDiv:
    case llvm::Instruction::FRem:
    case llvm::Instruction::FPTrunc:
    case llvm::Instruction::FPExt:
    case llvm::Instruction::UIToFP:
    case llvm::Instruction::SIToFP:
    case llvm::Instruction::FPToUI:
    case llvm::Instruction::FPToSI:
    case llvm::Instruction::FCmp:
        Logger::error("floating point ConstantExprs unsupported");
        assert(0);
    }
    llvm_unreachable("Unsupported expression in evalConstantExpr");
    return op1;
}

void Application::branch(std::shared_ptr<ExecutionState>& state, const std::vector<std::shared_ptr<Expr>>& conditions, std::vector<std::shared_ptr<ExecutionState>>& result, BranchType reason)
{
    unsigned N = conditions.size();
    assert(N);

    //if (!branchingPermitted(state)) {
    //    unsigned next = theRNG.getInt32() % N;
    //    for (unsigned i = 0; i < N; ++i) {
    //        if (i == next) {
    //            result.push_back(state);
    //        }
    //        else {
    //            result.push_back(nullptr);
    //        }
    //    }
    //    stats::inhibitedForks += N - 1;
    //}
    //else {
    //    stats::forks += N - 1;
    //    stats::incBranchStat(reason, N - 1);

    //    // XXX do proper balance or keep random?
    //    result.push_back(state);
    //    for (unsigned i = 1; i < N; ++i) {
    //        ExecutionState* es = result[theRNG.getInt32() % i];
    //        ExecutionState* ns = es->branch();
    //        addedStates.push_back(ns);
    //        result.push_back(ns);
    //        executionTree->attach(es->executionTreeNode, ns, es, reason);
    //    }
    //}

    //// If necessary redistribute seeds to match conditions, killing
    //// states if necessary due to OnlyReplaySeeds (inefficient but
    //// simple).

    //std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it =
    //    seedMap.find(&state);
    //if (it != seedMap.end()) {
    //    std::vector<SeedInfo> seeds = it->second;
    //    seedMap.erase(it);

    //    // Assume each seed only satisfies one condition (necessarily true
    //    // when conditions are mutually exclusive and their conjunction is
    //    // a tautology).
    //    for (std::vector<SeedInfo>::iterator siit = seeds.begin(),
    //        siie = seeds.end(); siit != siie; ++siit) {
    //        unsigned i;
    //        for (i = 0; i < N; ++i) {
    //            ref<ConstantExpr> res;
    //            bool success = solver->getValue(
    //                state.constraints, siit->assignment.evaluate(conditions[i]), res,
    //                state.queryMetaData);
    //            assert(success && "FIXME: Unhandled solver failure");
    //            (void)success;
    //            if (res->isTrue())
    //                break;
    //        }

    //        // If we didn't find a satisfying condition randomly pick one
    //        // (the seed will be patched).
    //        if (i == N)
    //            i = theRNG.getInt32() % N;

    //        // Extra check in case we're replaying seeds with a max-fork
    //        if (result[i])
    //            seedMap[result[i]].push_back(*siit);
    //    }

    //    if (OnlyReplaySeeds) {
    //        for (unsigned i = 0; i < N; ++i) {
    //            if (result[i] && !seedMap.count(result[i])) {
    //                terminateStateEarlyAlgorithm(*result[i], "Unseeded path during replay", StateTerminationType::Replay);
    //                result[i] = nullptr;
    //            }
    //        }
    //    }
    //}

    //for (unsigned i = 0; i < N; ++i)
    //    if (result[i]) addConstraint(result[i], conditions[i]);
}

void Application::executeInstructon(std::shared_ptr<ExecutionState>& state, std::shared_ptr<InstructionWrapper>& instructionWrapper)
{
    std::shared_ptr<StackFrame> stackFrame = state->getStackFrame();

    llvm::Instruction* i = instructionWrapper->instruction;
    switch (i->getOpcode()) {
        // Control flow
    case llvm::Instruction::Ret: {
        llvm::ReturnInst* ri = llvm::cast<llvm::ReturnInst>(i);
        llvm::Instruction* caller = nullptr;
        std::shared_ptr<Expr> result;
        if (ri->getNumOperands() != 0) std::shared_ptr<Expr> result = state->eval(instructionWrapper, 0);
        if (stackFrame->caller != stackFrame->function->instructions.end()) {
            std::shared_ptr<InstructionWrapper> wrapper= *(stackFrame->caller);
            if (wrapper) {
                caller = wrapper->instruction;
            }
        }

        if (state->getStackSize() <= 1) {
            assert(!caller && "caller set on initial stack frame");
            terminateStateOnExit(state);
        }
        else {
            state->popFrame();
            if (llvm::InvokeInst* ii = llvm::dyn_cast<llvm::InvokeInst>(caller)) {
                transferToBasicBlock(ii->getNormalDest(), caller->getParent(), state);
            }
            else {
                state->PC = stackFrame->caller;
                ++state->PC;
            }

            if (ri->getNumOperands() != 0) {
                llvm::Type* t = caller->getType();
                if (t != llvm::Type::getVoidTy(i->getContext())) {
                    // may need to do coercion due to bitcasts
                    uint64_t from = result->getWidth();
                    uint64_t to = m_moduleStorage->getTypeSizeInBits(t);

                    if (from != to) {
                        const llvm::CallBase& cb = llvm::cast<llvm::CallBase>(*caller);

                        // XXX need to check other param attrs ?
                        bool isSExt = cb.hasRetAttr(llvm::Attribute::SExt);
                        if (isSExt) {
                            result = std::make_shared<SExtExpr>(result, to);
                        }
                        else {
                            result = std::make_shared<ZExtExpr>(result, to);
                        }
                    }
                    state->bindLocal(*(stackFrame->caller), result);
                }
            }
            else {
                // We check that the return value has no users instead of
                // checking the type, since C defaults to returning int for
                // undeclared functions.
                if (!caller->use_empty()) {
                    terminateStateOnExecError(state, "return void when caller expected a result");
                }
            }
        }
        break;
    }
    case llvm::Instruction::Br: {
        llvm::BranchInst* bi = llvm::cast<llvm::BranchInst>(i);
        if (bi->isUnconditional()) {
            transferToBasicBlock(bi->getSuccessor(0), bi->getParent(), state);
        }
        else {
            // FIXME: Find a way that we don't have this hidden dependency.
            assert(bi->getCondition() == bi->getOperand(0) &&
                "Wrong operand index!");
            std::shared_ptr<Expr> cond = stackFrame->locals[0];

            //cond = optimizer.optimizeExpr(cond, false);
           auto branches = fork(state, cond, false, BranchType::Conditional);

            // NOTE: There is a hidden dependency here, markBranchVisited
            // requires that we still be in the context of the branch
            // instruction (it reuses its statistic id). Should be cleaned
            // up with convenient instruction specific data.
            //if (statsTracker && state.stack.back().kf->trackCoverage)
            //    statsTracker->markBranchVisited(branches.first, branches.second);

            if (branches.first)
                transferToBasicBlock(bi->getSuccessor(0), bi->getParent(), branches.first);
            if (branches.second)
                transferToBasicBlock(bi->getSuccessor(1), bi->getParent(), branches.second);
        }
        break;
    }
    case llvm::Instruction::IndirectBr: {
        // implements indirect branch to a label within the current function
        const auto bi = llvm::cast<llvm::IndirectBrInst>(i);
        std::shared_ptr<Expr> address = state->eval(instructionWrapper,0);
        address = toUnique(state, address);

        // concrete address
        if (const auto CE = std::dynamic_pointer_cast<ConstantExpr>(address)) {
            const auto bb_address = (llvm::BasicBlock*)CE->getZExtValue(sizeof(void*));
            transferToBasicBlock(bb_address, bi->getParent(), state);
            break;
        }

        // symbolic address
        const auto numDestinations = bi->getNumDestinations();
        std::vector<llvm::BasicBlock*> targets;
        targets.reserve(numDestinations);
        std::vector<std::shared_ptr<Expr>> expressions;
        expressions.reserve(numDestinations);

        std::shared_ptr<Expr> errorCase = std::make_shared<ConstantExpr>(1,1);
        llvm::SmallPtrSet<llvm::BasicBlock*, 5> destinations;
        // collect and check destinations from label list
        for (unsigned k = 0; k < numDestinations; ++k) {
            // filter duplicates
            const auto d = bi->getDestination(k);
            if (destinations.count(d)) continue;
            destinations.insert(d);

            // create address expression
            const auto PE = ConstantExpr::createPointer(reinterpret_cast<std::uint64_t>(d));
            std::shared_ptr<Expr> e = std::make_shared<EqExpr>(address, PE);

            // exclude address from errorCase
            errorCase = std::make_shared<AndExpr>(errorCase, Expr::createIsZero(e));

            // check feasibility
            bool result;
            bool success = m_solverManager.mayBeTrue(state->constraints, e, result, state->duraion);
            assert(success && "FIXME: Unhandled solver failure");
            if (result) {
                targets.push_back(d);
                expressions.push_back(e);
            }
        }
        // check errorCase feasibility
        bool result;
        bool success  = m_solverManager.mayBeTrue( state->constraints, errorCase, result, state->duraion);
        assert(success && "FIXME: Unhandled solver failure");
        if (result) {
            expressions.push_back(errorCase);
        }

        // fork states
        std::vector<std::shared_ptr<ExecutionState>> branches;
        branch(state, expressions, branches, BranchType::Indirect);

        // terminate error state
        if (result) {
            terminateStateOnExecError(branches.back(), "indirectbr: illegal label address");
            branches.pop_back();
        }

        // branch states to resp. target blocks
        assert(targets.size() == branches.size());
        for (std::vector<ExecutionState*>::size_type k = 0; k < branches.size(); ++k) {
            if (branches[k]) {
                transferToBasicBlock(targets[k], bi->getParent(), branches[k]);
            }
        }

        break;
    }
    case llvm::Instruction::Switch: {
        llvm::SwitchInst* si = llvm::cast<llvm::SwitchInst>(i);
        std::shared_ptr<Expr> cond = state->eval(instructionWrapper, 0);
        llvm::BasicBlock* bb = si->getParent();

        cond = toUnique(state, cond);
        if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(cond)) {
            // Somewhat gross to create these all the time, but fine till we
            // switch to an internal rep.
            llvm::IntegerType* Ty = cast<llvm::IntegerType>(si->getCondition()->getType());
            llvm::ConstantInt* ci = llvm::ConstantInt::get(Ty, CE->getZExtValue());
            unsigned index = si->findCaseValue(ci)->getSuccessorIndex();
            transferToBasicBlock(si->getSuccessor(index), si->getParent(), state);
        }
        else {
            // Handle possible different branch targets

            // We have the following assumptions:
            // - each case value is mutual exclusive to all other values
            // - order of case branches is based on the order of the expressions of
            //   the case values, still default is handled last
            std::vector<llvm::BasicBlock*> bbOrder;
            std::map<llvm::BasicBlock*, std::shared_ptr<Expr> > branchTargets;

            std::map<std::shared_ptr<Expr>, llvm::BasicBlock*> expressionOrder;

            // Iterate through all non-default cases and order them by expressions
            for (auto i : si->cases()) {
                std::shared_ptr<Expr> value = evalConstant(i.getCaseValue());

                llvm::BasicBlock* caseSuccessor = i.getCaseSuccessor();
                expressionOrder.insert(std::make_pair(value, caseSuccessor));
            }

            // Track default branch values
            std::shared_ptr<Expr> defaultValue = std::make_shared<ConstantExpr>(1,1);

            // iterate through all non-default cases but in order of the expressions
            for (std::map<std::shared_ptr<Expr>, llvm::BasicBlock*>::iterator
                it = expressionOrder.begin(),
                itE = expressionOrder.end();
                it != itE; ++it) {
                std::shared_ptr<Expr> match = std::make_shared<EqExpr>(cond, it->first);

                // skip if case has same successor basic block as default case
                // (should work even with phi nodes as a switch is a single terminating instruction)
                if (it->second == si->getDefaultDest()) continue;

                // Make sure that the default value does not contain this target's value
                defaultValue = std::make_shared<AndExpr>(defaultValue, Expr::createIsZero(match));

                // Check if control flow could take this case
                bool result;
                match = m_exprOptimizer.optimizeExpr(match, false);
                bool success = m_solverManager.mayBeTrue(state->constraints, match, result, state->duraion);
                assert(success && "FIXME: Unhandled solver failure");
                (void)success;
                if (result) {
                    llvm::BasicBlock* caseSuccessor = it->second;

                    // Handle the case that a basic block might be the target of multiple
                    // switch cases.
                    // Currently we generate an expression containing all switch-case
                    // values for the same target basic block. We spare us forking too
                    // many times but we generate more complex condition expressions
                    // TODO Add option to allow to choose between those behaviors
                    std::pair<std::map<llvm::BasicBlock*, std::shared_ptr<Expr> >::iterator, bool> res =
                        branchTargets.insert(std::make_pair(
                            caseSuccessor, std::make_shared<ConstantExpr>(1, 0)));

                    res.first->second = std::make_shared<OrExpr>(match, res.first->second);

                    // Only add basic blocks which have not been target of a branch yet
                    if (res.second) {
                        bbOrder.push_back(caseSuccessor);
                    }
                }
            }

            // Check if control could take the default case
            defaultValue = m_exprOptimizer.optimizeExpr(defaultValue, false);
            bool res;
            bool success = m_solverManager.mayBeTrue(state->constraints, defaultValue, res, state->duraion);
            assert(success && "FIXME: Unhandled solver failure");
            (void)success;
            if (res) {
                std::pair<std::map<llvm::BasicBlock*, std::shared_ptr<Expr> >::iterator, bool> ret =
                    branchTargets.insert(
                        std::make_pair(si->getDefaultDest(), defaultValue));
                if (ret.second) {
                    bbOrder.push_back(si->getDefaultDest());
                }
            }

            // Fork the current state with each state having one of the possible
            // successors of this switch
            std::vector< std::shared_ptr<Expr> > conditions;
            for (std::vector<llvm::BasicBlock*>::iterator it = bbOrder.begin(),
                ie = bbOrder.end();
                it != ie; ++it) {
                conditions.push_back(branchTargets[*it]);
            }
            std::vector<std::shared_ptr<ExecutionState>> branches;
            branch(state, conditions, branches, BranchType::Switch);

            std::vector<std::shared_ptr<ExecutionState>>::iterator bit = branches.begin();
            for (std::vector<llvm::BasicBlock*>::iterator it = bbOrder.begin(); it != bbOrder.end(); ++it) {
                std::shared_ptr<ExecutionState> es = *bit;
                if (es)
                    transferToBasicBlock(*it, bb, es);
                ++bit;
            }
        }
        break;
    }
    case llvm::Instruction::Unreachable:
        // Note that this is not necessarily an internal bug, llvm will
        // generate unreachable instructions in cases where it knows the
        // program will crash. So it is effectively a SEGV or internal
        // error.
        terminateStateOnExecError(state, "reached \"unreachable\" instruction");
        break;

    case llvm::Instruction::Invoke:
    case llvm::Instruction::Call: {
        // Ignore debug intrinsic calls
        if (llvm::isa<llvm::DbgInfoIntrinsic>(i))
            break;

        const llvm::CallBase& cb = cast<llvm::CallBase>(*i);
        llvm::Value* fp = cb.getCalledOperand();
        unsigned numArgs = cb.arg_size();
        llvm::Function* f = FunctionWrapper::getTargetFunction(fp);

        // evaluate arguments
        std::vector< std::shared_ptr<Expr> > arguments;
        arguments.reserve(numArgs);

        for (unsigned j = 0; j < numArgs; ++j)
            arguments.push_back( state->eval(instructionWrapper, j + 1));

        if (auto* asmValue = llvm::dyn_cast<llvm::InlineAsm>(fp)) { //TODO: move to `executeCall`
            //KInlineAsm callable(asmValue);
            //callExternalFunction(state, instructionWrapper, &callable, arguments);
            break;
        }

        if (f) {
            const llvm::FunctionType* fType = f->getFunctionType();
            const llvm::FunctionType* fpType = cb.getFunctionType();

            // special case the call with a bitcast case
            if (fType != fpType) {
                assert(fType && fpType && "unable to get function type");

                // XXX check result coercion

                // XXX this really needs thought and validation
                unsigned i = 0;
                for (std::vector< std::shared_ptr<Expr> >::iterator
                    ai = arguments.begin(), ie = arguments.end();
                    ai != ie; ++ai) {
                    uint64_t to, from = (*ai)->getWidth();

                    if (i < fType->getNumParams()) {
                        to = m_moduleStorage->getTypeSizeInBits(fType->getParamType(i));

                        if (from != to) {
                            // XXX need to check other param attrs ?
                            bool isSExt = cb.paramHasAttr(i, llvm::Attribute::SExt);
                            if (isSExt) {
                                arguments[i] = std::make_shared<SExtExpr>(arguments[i], to);
                            }
                            else {
                                arguments[i] = std::make_shared<ZExtExpr>(arguments[i], to);
                            }
                        }
                    }

                    i++;
                }
            }

            executeCall(state, instructionWrapper, f, arguments);
        }
        else {
            std::shared_ptr<Expr> v = state->eval(instructionWrapper, 0);

            bool hasInvalid = false, first = true;
            std::shared_ptr<ExecutionState> current = state;

            /* XXX This is wasteful, no need to do a full evaluate since we
               have already got a value. But in the end the caches should
               handle it for us, albeit with some overhead. */
            do {
                v = m_exprOptimizer.optimizeExpr(v, true);
                std::shared_ptr<ConstantExpr> value;
                bool success = m_solverManager.getValue(current->constraints, v, value, current->duraion);
                assert(success && "FIXME: Unhandled solver failure");
                (void)success;
                auto res = fork(state, std::make_shared<EqExpr>(v, value), true, BranchType::Call);
                if (res.first) {
                    uint64_t addr = value->getZExtValue();
                    auto it = m_legalFunctions.find(addr);
                    if (it != m_legalFunctions.end()) {
                        f = it->second;

                        // Don't give warning on unique resolution
                        if (res.second || !first) Logger::warn("{} resolved symbolic function pointer to: {}", addr, f->getName().data());
                        executeCall(res.first, instructionWrapper, f, arguments);
                    }
                    else {
                        if (!hasInvalid) {
                            terminateStateOnExecError(state, "invalid function pointer");
                            hasInvalid = true;
                        }
                    }
                }

                first = false;
                current = res.second;
            } while (current != nullptr);
        }
        break;
    }
    case llvm::Instruction::PHI: {
        std::shared_ptr<Expr> result = state->eval(instructionWrapper, state->incomingBBIndex);
        state->bindLocal(instructionWrapper, result);
        break;
    }

                         // Special instructions
    case llvm::Instruction::Select: {
        // NOTE: It is not required that operands 1 and 2 be of scalar type.
        std::shared_ptr<Expr> cond = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> tExpr = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> fExpr = state->eval(instructionWrapper, 2);
        std::shared_ptr<Expr> result = std::make_shared<SelectExpr>(cond, tExpr, fExpr);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::VAArg:
        terminateStateOnExecError(state, "unexpected VAArg instruction");
        break;

        // Arithmetic / logical

    case llvm::Instruction::Add: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        state->bindLocal(instructionWrapper, std::make_shared<AddExpr>(left, right));
        break;
    }

    case llvm::Instruction::Sub: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        state->bindLocal(instructionWrapper, std::make_shared<SubExpr>(left, right));
        break;
    }

    case llvm::Instruction::Mul: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        state->bindLocal(instructionWrapper, std::make_shared<MulExpr>(left, right));
        break;
    }

    case llvm::Instruction::UDiv: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<UDivExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::SDiv: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<SDivExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::URem: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<URemExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::SRem: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<SRemExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::And: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<AndExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::Or: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<OrExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::Xor: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<XorExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::Shl: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<ShlExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::LShr: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<LShrExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::AShr: {
        std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> result = std::make_shared<AShrExpr>(left, right);
        state->bindLocal(instructionWrapper, result);
        break;
    }

     // Compare
    case llvm::Instruction::ICmp: {
        llvm::CmpInst* ci = llvm::cast<llvm::CmpInst>(i);
        llvm::ICmpInst* ii = llvm::cast<llvm::ICmpInst>(ci);

        switch (ii->getPredicate()) {
        case llvm::ICmpInst::ICMP_EQ: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<EqExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_NE: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<NeExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_UGT: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<UgtExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_UGE: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<UgeExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_ULT: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<UltExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_ULE: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<UleExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_SGT: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<SgtExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_SGE: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<SgeExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_SLT: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<SltExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        case llvm::ICmpInst::ICMP_SLE: {
            std::shared_ptr<Expr> left = state->eval(instructionWrapper, 0);
            std::shared_ptr<Expr> right = state->eval(instructionWrapper, 1);
            std::shared_ptr<Expr> result = std::make_shared<SleExpr>(left, right);
            state->bindLocal(instructionWrapper, result);
            break;
        }

        default:
            terminateStateOnExecError(state, "invalid ICmp predicate");
        }
        break;
    }

                          // Memory instructions...
    case llvm::Instruction::Alloca: {
        llvm::AllocaInst* ai = llvm::cast<llvm::AllocaInst>(i);
        uint32_t elementSize = m_moduleStorage->getTypeSize(ai->getAllocatedType());
        std::shared_ptr<Expr> size = ConstantExpr::createPointer(elementSize);
        if (ai->isArrayAllocation()) {
            std::shared_ptr<Expr> count = state->eval(instructionWrapper, 0);
            count = std::make_shared<ZExtExpr>(count,sizeof(void*));
            size = std::make_shared<MulExpr>(size, count);
        }
        executeAlloc(state, size, true, instructionWrapper);
        break;
    }

    case llvm::Instruction::Load: {
        std::shared_ptr<Expr> base = state->eval(instructionWrapper, 0);
        executeMemoryOperation(state, false, base, 0, instructionWrapper);
        break;
    }
    case llvm::Instruction::Store: {
        std::shared_ptr<Expr> base = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> value = state->eval(instructionWrapper, 0);
        executeMemoryOperation(state, true, base, value, 0);
        break;
    }

    case llvm::Instruction::GetElementPtr: {
        std::shared_ptr<Expr> base = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> original_base = base;

        for (std::vector< std::pair<unsigned, uint64_t> >::iterator
            it = instructionWrapper->indices.begin(), ie = instructionWrapper->indices.end();
            it != ie; ++it) {
            uint64_t elementSize = it->second;
            std::shared_ptr<Expr> index = state->eval(instructionWrapper, it->first);
            base = std::make_shared<AddExpr>(base, std::make_shared<MulExpr>(std::make_shared<SExtExpr>(index,sizeof(void*)),
                    ConstantExpr::createPointer(elementSize)));
        }
        if (instructionWrapper->offset)
            base = std::make_shared<AddExpr>(base, ConstantExpr::createPointer(instructionWrapper->offset));

        state->bindLocal(instructionWrapper, base);
        break;
    }

                                   // Conversion
    case llvm::Instruction::Trunc: {
        llvm::CastInst* ci = llvm::cast<llvm::CastInst>(i);
        std::shared_ptr<Expr> result = std::make_shared<ExtractExpr>(state->eval(instructionWrapper, 0),
            0,
            m_moduleStorage->getTypeSizeInBits(ci->getType()));
        state->bindLocal(instructionWrapper, result);
        break;
    }
    case llvm::Instruction::ZExt: {
        llvm::CastInst* ci = llvm::cast<llvm::CastInst>(i);
        std::shared_ptr<Expr> result = std::make_shared<ZExtExpr>(state->eval(instructionWrapper, 0),
            m_moduleStorage->getTypeSizeInBits(ci->getType()));
        state->bindLocal(instructionWrapper, result);
        break;
    }
    case llvm::Instruction::SExt: {
        llvm::CastInst* ci = llvm::cast<llvm::CastInst>(i);
        std::shared_ptr<Expr> result = std::make_shared<SExtExpr>(state->eval(instructionWrapper, 0),m_moduleStorage->getTypeSizeInBits(ci->getType()));
        state->bindLocal(instructionWrapper, result);
        break;
    }

    case llvm::Instruction::IntToPtr: {
        llvm::CastInst* ci = llvm::cast<llvm::CastInst>(i);
        uint64_t pType = m_moduleStorage->getTypeSizeInBits(ci->getType());
        std::shared_ptr<Expr> arg = state->eval(instructionWrapper, 0);
        state->bindLocal(instructionWrapper, std::make_shared<ZExtExpr>(arg, pType));
        break;
    }
    case llvm::Instruction::PtrToInt: {
        llvm::CastInst* ci = llvm::cast<llvm::CastInst>(i);
        uint64_t iType = m_moduleStorage->getTypeSizeInBits(ci->getType());
        std::shared_ptr<Expr> arg = state->eval(instructionWrapper, 0);
        state->bindLocal(instructionWrapper, std::make_shared<ZExtExpr>(arg, iType));
        break;
    }

    case llvm::Instruction::BitCast: {
        std::shared_ptr<Expr> result = state->eval(instructionWrapper, 0);
        state->bindLocal(instructionWrapper, result);
        break;
    }

                             // Floating point instructions
    case llvm::Instruction::FNeg: {
        std::shared_ptr<ConstantExpr> arg =
            toConstant(state, state->eval(instructionWrapper, 0), "floating point");
        if (!Expr::fpWidthToSemantics(arg->getWidth()))
            return terminateStateOnExecError(state, "Unsupported FNeg operation");

        llvm::APFloat Res(*Expr::fpWidthToSemantics(arg->getWidth()), arg->getAPValue());
        Res = llvm::neg(Res);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FAdd: {
        std::shared_ptr<ConstantExpr> left = toConstant(state, state->eval(instructionWrapper, 0), "floating point");
        std::shared_ptr<ConstantExpr> right = toConstant(state, state->eval(instructionWrapper, 1), "floating point");
        if (!Expr::fpWidthToSemantics(left->getWidth()) ||
            !Expr::fpWidthToSemantics(right->getWidth()))
            return terminateStateOnExecError(state, "Unsupported FAdd operation");

        llvm::APFloat Res(*Expr::fpWidthToSemantics(left->getWidth()), left->getAPValue());
        Res.add(llvm::APFloat(*Expr::fpWidthToSemantics(right->getWidth()), right->getAPValue()), llvm::APFloat::rmNearestTiesToEven);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FSub: {
        std::shared_ptr<ConstantExpr> left = toConstant(state, state->eval(instructionWrapper, 0), "floating point");
        std::shared_ptr<ConstantExpr> right = toConstant(state, state->eval(instructionWrapper, 1), "floating point");
        if (!Expr::fpWidthToSemantics(left->getWidth()) || !Expr::fpWidthToSemantics(right->getWidth()))
            return terminateStateOnExecError(state, "Unsupported FSub operation");
        llvm::APFloat Res(*Expr::fpWidthToSemantics(left->getWidth()), left->getAPValue());
        Res.subtract(llvm::APFloat(*Expr::fpWidthToSemantics(right->getWidth()), right->getAPValue()), llvm::APFloat::rmNearestTiesToEven);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FMul: {
        std::shared_ptr<ConstantExpr> left = toConstant(state, state->eval(instructionWrapper, 0),
            "floating point");
        std::shared_ptr<ConstantExpr> right = toConstant(state, state->eval(instructionWrapper, 1),
            "floating point");
        if (!Expr::fpWidthToSemantics(left->getWidth()) ||
            !Expr::fpWidthToSemantics(right->getWidth()))
            return terminateStateOnExecError(state, "Unsupported FMul operation");

        llvm::APFloat Res(*Expr::fpWidthToSemantics(left->getWidth()), left->getAPValue());
        Res.multiply(llvm::APFloat(*Expr::fpWidthToSemantics(right->getWidth()), right->getAPValue()), llvm::APFloat::rmNearestTiesToEven);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FDiv: {
        std::shared_ptr<ConstantExpr> left = toConstant(state, state->eval(instructionWrapper, 0), "floating point");
        std::shared_ptr<ConstantExpr> right = toConstant(state, state->eval(instructionWrapper, 1), "floating point");
        if (!Expr::fpWidthToSemantics(left->getWidth()) || !Expr::fpWidthToSemantics(right->getWidth()))
            return terminateStateOnExecError(state, "Unsupported FDiv operation");

        llvm::APFloat Res(*Expr::fpWidthToSemantics(left->getWidth()), left->getAPValue());
        Res.divide(llvm::APFloat(*Expr::fpWidthToSemantics(right->getWidth()), right->getAPValue()), llvm::APFloat::rmNearestTiesToEven);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FRem: {
        std::shared_ptr<ConstantExpr> left = toConstant(state, state->eval(instructionWrapper, 0),
            "floating point");
        std::shared_ptr<ConstantExpr> right = toConstant(state, state->eval(instructionWrapper, 1),
            "floating point");
        if (!Expr::fpWidthToSemantics(left->getWidth()) ||
            !Expr::fpWidthToSemantics(right->getWidth()))
            return terminateStateOnExecError(state, "Unsupported FRem operation");
        llvm::APFloat Res(*Expr::fpWidthToSemantics(left->getWidth()), left->getAPValue());
        Res.mod(
            llvm::APFloat(*Expr::fpWidthToSemantics(right->getWidth()), right->getAPValue()));
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FPTrunc: {
        llvm::FPTruncInst* fi = llvm::cast<llvm::FPTruncInst>(i);
        uint64_t resultType = m_moduleStorage->getTypeSizeInBits(fi->getType());
        std::shared_ptr<ConstantExpr> arg = toConstant(state, state->eval(instructionWrapper, 0),
            "floating point");
        if (!Expr::fpWidthToSemantics(arg->getWidth()) || resultType > arg->getWidth())
            return terminateStateOnExecError(state, "Unsupported FPTrunc operation");

        llvm::APFloat Res(*Expr::fpWidthToSemantics(arg->getWidth()), arg->getAPValue());
        bool losesInfo = false;
        Res.convert(*Expr::fpWidthToSemantics(resultType),
            llvm::APFloat::rmNearestTiesToEven,
            &losesInfo);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FPExt: {
        llvm::FPExtInst* fi = llvm::cast<llvm::FPExtInst>(i);
        uint64_t resultType = m_moduleStorage->getTypeSizeInBits(fi->getType());
        std::shared_ptr<ConstantExpr> arg = toConstant(state, state->eval(instructionWrapper, 0), "floating point");
        if (!Expr::fpWidthToSemantics(arg->getWidth()) || arg->getWidth() > resultType)
            return terminateStateOnExecError(state, "Unsupported FPExt operation");
        llvm::APFloat Res(*Expr::fpWidthToSemantics(arg->getWidth()), arg->getAPValue());
        bool losesInfo = false;
        Res.convert(*Expr::fpWidthToSemantics(resultType),
            llvm::APFloat::rmNearestTiesToEven,
            &losesInfo);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FPToUI: {
        llvm::FPToUIInst* fi = llvm::cast<llvm::FPToUIInst>(i);
        uint64_t resultType = m_moduleStorage->getTypeSizeInBits(fi->getType());
        std::shared_ptr<ConstantExpr> arg = toConstant(state, state->eval(instructionWrapper, 0),
            "floating point");
        if (!Expr::fpWidthToSemantics(arg->getWidth()) || resultType > 64)
            return terminateStateOnExecError(state, "Unsupported FPToUI operation");

        llvm::APFloat Arg(*Expr::fpWidthToSemantics(arg->getWidth()), arg->getAPValue());
        uint64_t value = 0;
        bool isExact = true;
        auto valueRef = llvm::MutableArrayRef(value);

        Arg.convertToInteger(valueRef, resultType, false,
            llvm::APFloat::rmTowardZero, &isExact);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(llvm::APInt(resultType,value)));
        break;
    }

    case llvm::Instruction::FPToSI: {
        llvm::FPToSIInst* fi = llvm::cast<llvm::FPToSIInst>(i);
        uint64_t resultType = m_moduleStorage->getTypeSizeInBits(fi->getType());
        std::shared_ptr<ConstantExpr> arg = toConstant(state, state->eval(instructionWrapper, 0),
            "floating point");
        if (!Expr::fpWidthToSemantics(arg->getWidth()) || resultType > 64)
            return terminateStateOnExecError(state, "Unsupported FPToSI operation");
        llvm::APFloat Arg(*Expr::fpWidthToSemantics(arg->getWidth()), arg->getAPValue());

        uint64_t value = 0;
        bool isExact = true;
        auto valueRef = llvm::MutableArrayRef(value);
        Arg.convertToInteger(valueRef, resultType, true,
            llvm::APFloat::rmTowardZero, &isExact);
        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(llvm::APInt(resultType, value)));
        break;
    }

    case llvm::Instruction::UIToFP: {
        llvm::UIToFPInst* fi = llvm::cast<llvm::UIToFPInst>(i);
        uint64_t resultType = m_moduleStorage->getTypeSizeInBits(fi->getType());
        std::shared_ptr<ConstantExpr> arg = toConstant(state, state->eval(instructionWrapper, 0),
            "floating point");
        const llvm::fltSemantics* semantics = Expr::fpWidthToSemantics(resultType);
        if (!semantics)
            return terminateStateOnExecError(state, "Unsupported UIToFP operation");
        llvm::APFloat f(*semantics, 0);
        f.convertFromAPInt(arg->getAPValue(), false,
            llvm::APFloat::rmNearestTiesToEven);

        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(f.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::SIToFP: {
        llvm::SIToFPInst* fi = llvm::cast<llvm::SIToFPInst>(i);
        uint64_t resultType = m_moduleStorage->getTypeSizeInBits(fi->getType());
        std::shared_ptr<ConstantExpr> arg = toConstant(state, state->eval(instructionWrapper, 0), "floating point");
        const llvm::fltSemantics* semantics = Expr::fpWidthToSemantics(resultType);
        if (!semantics)
            return terminateStateOnExecError(state, "Unsupported SIToFP operation");
        llvm::APFloat f(*semantics, 0);
        f.convertFromAPInt(arg->getAPValue(), true,
            llvm::APFloat::rmNearestTiesToEven);

        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(f.bitcastToAPInt()));
        break;
    }

    case llvm::Instruction::FCmp: {
        llvm::FCmpInst* fi = llvm::cast<llvm::FCmpInst>(i);
        std::shared_ptr<ConstantExpr> left = toConstant(state, state->eval(instructionWrapper, 0),
            "floating point");
        std::shared_ptr<ConstantExpr> right = toConstant(state, state->eval(instructionWrapper, 1),
            "floating point");
        if (!Expr::fpWidthToSemantics(left->getWidth()) ||
            !Expr::fpWidthToSemantics(right->getWidth()))
            return terminateStateOnExecError(state, "Unsupported FCmp operation");

        llvm::APFloat LHS(*Expr::fpWidthToSemantics(left->getWidth()), left->getAPValue());
        llvm::APFloat RHS(*Expr::fpWidthToSemantics(right->getWidth()), right->getAPValue());
        llvm::APFloat::cmpResult CmpRes = LHS.compare(RHS);

        bool Result = false;
        switch (fi->getPredicate()) {
            // Predicates which only care about whether or not the operands are NaNs.
        case llvm::FCmpInst::FCMP_ORD:
            Result = (CmpRes != llvm::APFloat::cmpUnordered);
            break;

        case llvm::FCmpInst::FCMP_UNO:
            Result = (CmpRes == llvm::APFloat::cmpUnordered);
            break;

            // Ordered comparisons return false if either operand is NaN.  Unordered
            // comparisons return true if either operand is NaN.
        case llvm::FCmpInst::FCMP_UEQ:
            Result = (CmpRes == llvm::APFloat::cmpUnordered || CmpRes == llvm::APFloat::cmpEqual);
            break;
        case llvm::FCmpInst::FCMP_OEQ:
            Result = (CmpRes != llvm::APFloat::cmpUnordered && CmpRes == llvm::APFloat::cmpEqual);
            break;

        case llvm::FCmpInst::FCMP_UGT:
            Result = (CmpRes == llvm::APFloat::cmpUnordered || CmpRes == llvm::APFloat::cmpGreaterThan);
            break;
        case llvm::FCmpInst::FCMP_OGT:
            Result = (CmpRes != llvm::APFloat::cmpUnordered && CmpRes == llvm::APFloat::cmpGreaterThan);
            break;

        case llvm::FCmpInst::FCMP_UGE:
            Result = (CmpRes == llvm::APFloat::cmpUnordered || (CmpRes == llvm::APFloat::cmpGreaterThan || CmpRes == llvm::APFloat::cmpEqual));
            break;
        case llvm::FCmpInst::FCMP_OGE:
            Result = (CmpRes != llvm::APFloat::cmpUnordered && (CmpRes == llvm::APFloat::cmpGreaterThan || CmpRes == llvm::APFloat::cmpEqual));
            break;

        case llvm::FCmpInst::FCMP_ULT:
            Result = (CmpRes == llvm::APFloat::cmpUnordered || CmpRes == llvm::APFloat::cmpLessThan);
            break;
        case llvm::FCmpInst::FCMP_OLT:
            Result = (CmpRes != llvm::APFloat::cmpUnordered && CmpRes == llvm::APFloat::cmpLessThan);
            break;

        case llvm::FCmpInst::FCMP_ULE:
            Result = (CmpRes == llvm::APFloat::cmpUnordered || (CmpRes == llvm::APFloat::cmpLessThan || CmpRes == llvm::APFloat::cmpEqual));
            break;
        case llvm::FCmpInst::FCMP_OLE:
            Result = (CmpRes != llvm::APFloat::cmpUnordered && (CmpRes == llvm::APFloat::cmpLessThan || CmpRes == llvm::APFloat::cmpEqual));
            break;

        case llvm::FCmpInst::FCMP_UNE:
            Result = (CmpRes == llvm::APFloat::cmpUnordered || CmpRes != llvm::APFloat::cmpEqual);
            break;
        case llvm::FCmpInst::FCMP_ONE:
            Result = (CmpRes != llvm::APFloat::cmpUnordered && CmpRes != llvm::APFloat::cmpEqual);
            break;

        default:
            assert(0 && "Invalid FCMP predicate!");
            break;
        case llvm::FCmpInst::FCMP_FALSE:
            Result = false;
            break;
        case llvm::FCmpInst::FCMP_TRUE:
            Result = true;
            break;
        }

        state->bindLocal(instructionWrapper, std::make_shared<ConstantExpr>(1,Result));
        break;
    }
    case llvm::Instruction::InsertValue: {
        std::shared_ptr<Expr> agg = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> val = state->eval(instructionWrapper, 1);

        std::shared_ptr<Expr> l = NULL, r = NULL;
        unsigned lOffset = instructionWrapper->offset * 8, rOffset = instructionWrapper->offset * 8 + val->getWidth();

        if (lOffset > 0)  l = std::make_shared<ExtractExpr>(agg, 0, lOffset);
        if (rOffset < agg->getWidth())
            r = std::make_shared<ExtractExpr>(agg, rOffset, agg->getWidth() - rOffset);

        std::shared_ptr<Expr> result;
        if (l && r)
            result = std::make_shared<ConcatExpr>(r, std::make_shared<ConcatExpr>(val, l));
        else if (l)
            result = std::make_shared<ConcatExpr>(val, l);
        else if (r)
            result = std::make_shared<ConcatExpr>(r, val);
        else
            result = val;

        state->bindLocal(instructionWrapper, result);
        break;
    }
    case llvm::Instruction::ExtractValue: {
        std::shared_ptr<Expr> agg = state->eval(instructionWrapper, 0);

        std::shared_ptr<Expr> result = std::make_shared<ExtractExpr>(agg, instructionWrapper->offset * 8, m_moduleStorage->getTypeSizeInBits(i->getType()));

        state->bindLocal(instructionWrapper, result);
        break;
    }
    case llvm::Instruction::Fence: {
        // Ignore for now4
        break;
    }
    case llvm::Instruction::InsertElement: {
        llvm::InsertElementInst* iei = llvm::cast<llvm::InsertElementInst>(i);
        std::shared_ptr<Expr> vec = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> newElt = state->eval(instructionWrapper, 1);
        std::shared_ptr<Expr> idx = state->eval(instructionWrapper, 2);

        std::shared_ptr<ConstantExpr> cIdx = std::dynamic_pointer_cast<ConstantExpr>(idx);
        if (cIdx == NULL) {
            terminateStateOnExecError(state, "InsertElement, support for symbolic index not implemented");
            return;
        }
        uint64_t iIdx = cIdx->getZExtValue();
        const auto* vt = cast<llvm::FixedVectorType>(iei->getType());
        unsigned EltBits = m_moduleStorage->getTypeSizeInBits(vt->getElementType());

        if (iIdx >= vt->getNumElements()) {
            // Out of bounds write
            terminateStateOnProgramError(state, "Out of bounds write when inserting element",  StateTerminationType::BadVectorAccess);
            return;
        }

        std::vector<std::shared_ptr<Expr>> elems;
        for (unsigned i = vt->getNumElements(); i != 0; --i) {
            auto of = i - 1;
            unsigned bitOffset = EltBits * of;
            elems.push_back(of == iIdx ? newElt : std::make_shared<ExtractExpr>(vec, bitOffset, EltBits));
        }

        //assert(Context::get().isLittleEndian() && "FIXME:Broken for big endian");
        std::shared_ptr<Expr> Result = ConcatExpr::concat(elems);
        state->bindLocal(instructionWrapper, Result);
        break;
    }
    case llvm::Instruction::ExtractElement: {
        llvm::ExtractElementInst* eei = llvm::cast<llvm::ExtractElementInst>(i);
        std::shared_ptr<Expr> vec = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> idx = state->eval(instructionWrapper, 1);

        std::shared_ptr<ConstantExpr> cIdx = std::dynamic_pointer_cast<ConstantExpr>(idx);
        if (cIdx == NULL) {
            terminateStateOnExecError(state, "ExtractElement, support for symbolic index not implemented");
            return;
        }
        uint64_t iIdx = cIdx->getZExtValue();
        const auto* vt = cast<llvm::FixedVectorType>(eei->getVectorOperandType());
        unsigned EltBits = m_moduleStorage->getTypeSizeInBits(vt->getElementType());

        if (iIdx >= vt->getNumElements()) {
            // Out of bounds read
            terminateStateOnProgramError(state, "Out of bounds read when extracting element", StateTerminationType::BadVectorAccess);
            return;
        }

        unsigned bitOffset = EltBits * iIdx;
        std::shared_ptr<Expr> Result = std::make_shared<ExtractExpr>(vec, bitOffset, EltBits);
        state->bindLocal(instructionWrapper, Result);
        break;
    }
    case llvm::Instruction::ShuffleVector:
        // Should never happen due to Scalarizer pass removing ShuffleVector
        // instructions.
        terminateStateOnExecError(state, "Unexpected ShuffleVector instruction");
        break;

#ifdef SUPPORT_KLEE_EH_CXX
    case llvm::Instruction::Resume: {
        auto* cui = dyn_cast_or_null<CleanupPhaseUnwindingInformation>(
            state.unwindingInformation.get());

        if (!cui) {
            terminateStateOnError(
                state,
                "resume-instruction executed outside of cleanup phase unwinding");
            break;
        }

        std::shared_ptr<Expr> arg = state->eval(instructionWrapper, 0);
        std::shared_ptr<Expr> exceptionPointer = ExtractExpr::create(arg, 0, Expr::Int64);
        std::shared_ptr<Expr> selectorValue =
            ExtractExpr::create(arg, Expr::Int64, Expr::Int32);

        if (!dyn_cast<ConstantExpr>(exceptionPointer) ||
            !dyn_cast<ConstantExpr>(selectorValue)) {
            terminateStateOnExecError(
                state, "resume-instruction called with non constant expression");
            break;
        }

        if (!Expr::createIsZero(selectorValue)->isTrue()) {
            klee_warning("resume-instruction called with non-0 selector value");
        }

        if (!EqExpr::create(exceptionPointer, cui->exceptionObject)->isTrue()) {
            terminateStateOnExecError(
                state, "resume-instruction called with unexpected exception pointer");
            break;
        }

        unwindToNextLandingpad(state);
        break;
    }

    case llvm::Instruction::LandingPad: {
        auto* cui = dyn_cast_or_null<CleanupPhaseUnwindingInformation>(
            state.unwindingInformation.get());

        if (!cui) {
            terminateStateOnExecError(
                state, "Executing landing pad but not in unwinding phase 2");
            break;
        }

        std::shared_ptr<ConstantExpr> exceptionPointer = cui->exceptionObject;
        std::shared_ptr<ConstantExpr> selectorValue;

        // check on which frame we are currently
        if (state.stack.size() - 1 == cui->catchingStackIndex) {
            // we are in the target stack frame, return the selector value
            // that was returned by the personality fn in phase 1 and stop unwinding.
            selectorValue = cui->selectorValue;

            // stop unwinding by cleaning up our unwinding information.
            state.unwindingInformation.reset();

            // this would otherwise now be a dangling pointer
            cui = nullptr;
        }
        else {
            // we are not yet at the target stack frame. the landingpad might have
            // a cleanup clause or not, anyway, we give it the selector value "0",
            // which represents a cleanup, and expect it to handle it.
            // This is explicitly allowed by LLVM, see
            // https://llvm.org/docs/ExceptionHandling.html#id18
            selectorValue = std::make_shared<ConstantExpr>(32,0);
        }

        // we have to return a {i8*, i32}
        std::shared_ptr<Expr> result = ConcatExpr::create(
            ZExtExpr::create(selectorValue, Expr::Int32), exceptionPointer);

        state->bindLocal(instructionWrapper, result);

        break;
    }
#endif // SUPPORT_KLEE_EH_CXX

    case llvm::Instruction::AtomicRMW:
        terminateStateOnExecError(state, "Unexpected Atomic instruction, should be lowered by LowerAtomicInstructionPass");
        break;
    case llvm::Instruction::AtomicCmpXchg:
        terminateStateOnExecError(state, "Unexpected AtomicCmpXchg instruction, should be lowered by LowerAtomicInstructionPass");
        break;
        // Other instructions...
        // Unhandled
    default:
        terminateStateOnExecError(state, "illegal instruction");
        break;
    }
}

void Application::terminateStateOnExit(std::shared_ptr<ExecutionState>& state)
{
    StatisticManager::terminateOnExit++;

}

void Application::terminateStateOnExecError(std::shared_ptr<ExecutionState>& state, const std::string& message, StateTerminationType reason)
{
    assert(reason > StateTerminationType::User && reason <= StateTerminationType::External);
    StatisticManager::terminationExecutionError++;
    terminateStateOnError(state, message, reason);
}

void Application::terminateStateOnProgramError(std::shared_ptr<ExecutionState>& state, const std::string& message, StateTerminationType reason)
{
    assert(reason > StateTerminationType::Solver && reason <= StateTerminationType::NullableAttribute);
    StatisticManager::terminationExecutionError++;
    terminateStateOnError(state, message, reason);
}

void Application::terminateStateOnError(std::shared_ptr<ExecutionState>& state, const std::string& message, StateTerminationType terminationType)
{
    static std::set< std::pair<llvm::Instruction*, std::string> > emittedErrors;
    std::shared_ptr<InstructionWrapper> lastInst;
    const std::shared_ptr<InstructionWrapper> ii = getLastNonKleeInternalInstruction(state,lastInst);

    if (
        emittedErrors.insert(std::make_pair(lastInst->instruction, message)).second) {
        if (!ii->file.empty()) {
            Logger::error("ERROR: {}: {}: {}", ii->file.c_str(), ii->line, message.c_str());
        }
        else {
            Logger::error("ERROR: (location information missing) {}", message.c_str());
        }

        std::string MsgString;
        llvm::raw_string_ostream msg(MsgString);
        msg << "Error: " << message << '\n';
        if (!ii->file.empty()) {
            msg << "File: " << ii->file << '\n'
                << "Line: " << ii->line << '\n'
                << "assembly.ll line: " << ii->assemblyLine << '\n'
                << "State: " << state->ID() << '\n';
        }
        msg << "Stack: \n";
        state->dumpStack(msg);

        processTestCase(state, msg.str().c_str());
    }

    terminateState(state, terminationType);

    if (shouldExitOn(terminationType))
        m_haltExecution = true;
}

void Application::transferToBasicBlock(llvm::BasicBlock* dst, llvm::BasicBlock* src, std::shared_ptr<ExecutionState>& state)
{
    std::shared_ptr<StackFrame> stackFrame = state->getStackFrame();
    std::shared_ptr<FunctionWrapper> kf = stackFrame->function;
    unsigned entry = kf->basicBlockMap[dst] - 1;
    state->PC = kf->instructions.begin() += entry;
    std::shared_ptr<InstructionWrapper> wrapper = *state->PC;
    if (wrapper->getOpcode() == llvm::Instruction::PHI) {
        llvm::PHINode* first = static_cast<llvm::PHINode*>(wrapper->instruction);
        state->incomingBBIndex = first->getBasicBlockIndex(src);
    }
}

std::pair< std::shared_ptr<ExecutionState>, std::shared_ptr<ExecutionState>> Application::fork(std::shared_ptr<ExecutionState>& currentState,const std::shared_ptr<Expr>& condition, bool isInternal, BranchType type)
{
    return { nullptr,nullptr };
}

std::shared_ptr<Expr> Application::toUnique(std::shared_ptr<ExecutionState>& currentState, std::shared_ptr<Expr> expr)
{
    std::shared_ptr<ConstantExpr> value;
    bool isTrue = false;
    //e = optimizer.optimizeExpr(e, true);
    //solver->setTimeout(coreSolverTimeout);
    //if (solver->getValue(state.constraints, e, value, state.queryMetaData)) {
    //    ref<Expr> cond = EqExpr::create(e, value);
    //    cond = optimizer.optimizeExpr(cond, false);
    //    if (solver->mustBeTrue(state.constraints, cond, isTrue,
    //        state.queryMetaData) &&
    //        isTrue)
    //        result = value;
    //}
    //solver->setTimeout(time::Span());
    return value;
}

std::shared_ptr<ConstantExpr> Application::toConstant(const std::shared_ptr<ExecutionState>& state, std::shared_ptr<Expr> expr, bool concretize)
{
    //e = ConstraintManager::simplifyExpr(state.constraints, e);
    //if (ConstantExpr* CE = dyn_cast<ConstantExpr>(e))
    //    return CE;

    /* If no seed evaluation results in a constant, call the solver */
    //ref<ConstantExpr> cvalue = getValueFromSeeds(state, e);
    //if (!cvalue) {
    //    [[maybe_unused]] bool success =
    //        solver->getValue(state.constraints, e, cvalue, state.queryMetaData);
    //    assert(success && "FIXME: Unhandled solver failure");
    //}

    //std::string str;
    //llvm::raw_string_ostream os(str);
    //os << "silently concretizing (reason: " << reason << ") expression " << e
    //    << " to value " << cvalue << " (" << (*(state.pc)).info->file << ":"
    //    << (*(state.pc)).info->line << ")";

    //if (ExternalCallWarnings == ExtCallWarnings::All)
    //    klee_warning("%s", os.str().c_str());
    //else
    //    klee_warning_once(reason.c_str(), "%s", os.str().c_str());

    //if (concretize)
    //    addConstraint(state, EqExpr::create(e, cvalue));

    return nullptr;
}

void Application::executeCall(std::shared_ptr<ExecutionState>& state, const std::shared_ptr<InstructionWrapper>& wrapper, llvm::Function* func, std::vector<std::shared_ptr<Expr>>& arguments)
{
    llvm::Instruction* i = wrapper->instruction;
    if (isa_and_nonnull<llvm::DbgInfoIntrinsic>(i))
        return;
    if (func && func->isDeclaration()) {
        switch (func->getIntrinsicID()) {
        case llvm::Intrinsic::not_intrinsic: {
            // state may be destroyed by this call, cannot touch
            std::shared_ptr<FunctionWrapper> function = m_moduleStorage->getFunction(func);
            callExternalFunction(state, wrapper, function, arguments);
            break;
        }
        case llvm::Intrinsic::fabs: {
            std::shared_ptr<ConstantExpr> arg =
                toConstant(state, arguments[0], "floating point");
            if (!Expr::fpWidthToSemantics(arg->getWidth()))
                return terminateStateOnExecError(
                    state, "Unsupported intrinsic llvm.fabs call");

            llvm::APFloat Res(*Expr::fpWidthToSemantics(arg->getWidth()),
                arg->getAPValue());
            Res = llvm::abs(Res);

            state->bindLocal(wrapper, std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
            break;
        }

        case llvm::Intrinsic::fma:
        case llvm::Intrinsic::fmuladd: {
            // Both fma and fmuladd support float, double and fp80.  Note, that fp80
            // is not mentioned in the documentation of fmuladd, nevertheless, it is
            // still supported.  For details see
            // https://github.com/klee/klee/pull/1507/files#r894993332

            if (isa<llvm::VectorType>(i->getOperand(0)->getType()))
                return terminateStateOnExecError( state, func->getName().str() + " with vectors is not supported");

            std::shared_ptr<ConstantExpr> op1 =
                toConstant(state, state->eval(wrapper, 1), "floating point");
            std::shared_ptr<ConstantExpr> op2 =
                toConstant(state, state->eval(wrapper, 2), "floating point");
            std::shared_ptr<ConstantExpr> op3 =
                toConstant(state, state->eval(wrapper, 3), "floating point");

            if (!Expr::fpWidthToSemantics(op1->getWidth()) ||
                !Expr::fpWidthToSemantics(op2->getWidth()) ||
                !Expr::fpWidthToSemantics(op3->getWidth()))
                return terminateStateOnExecError(state, "Unsupported " + func->getName().str() + " call");

            // (op1 * op2) + op3
            llvm::APFloat Res(*Expr::fpWidthToSemantics(op1->getWidth()), op1->getAPValue());
            Res.fusedMultiplyAdd(
                llvm::APFloat(*Expr::fpWidthToSemantics(op2->getWidth()), op2->getAPValue()),
                llvm::APFloat(*Expr::fpWidthToSemantics(op3->getWidth()), op3->getAPValue()),
                llvm::APFloat::rmNearestTiesToEven);

            state->bindLocal(wrapper , std::make_shared<ConstantExpr>(Res.bitcastToAPInt()));
            break;
        }
        case llvm::Intrinsic::abs: {
            if (isa<llvm::VectorType>(i->getOperand(0)->getType()))
                return terminateStateOnExecError(
                    state, "llvm.abs with vectors is not supported");

            std::shared_ptr<Expr> op = state->eval(wrapper,1);
            std::shared_ptr<Expr> poison = state->eval(wrapper, 2);

            assert(poison->getWidth() == 1 && "Second argument is not an i1");
            unsigned bw = op->getWidth();

            uint64_t moneVal = llvm::APInt(bw, -1, true).getZExtValue();
            uint64_t sminVal = llvm::APInt::getSignedMinValue(bw).getZExtValue();

            std::shared_ptr<ConstantExpr> zero = std::make_shared < ConstantExpr>(bw,0);
            std::shared_ptr<ConstantExpr> mone = std::make_shared < ConstantExpr>(bw,moneVal);
            std::shared_ptr<ConstantExpr> smin = std::make_shared < ConstantExpr>(bw,sminVal);

            std::shared_ptr<ConstantExpr> poisonConstExpr = std::dynamic_pointer_cast<ConstantExpr>(poison);
            if (poisonConstExpr && poisonConstExpr->isTrue()) {
                std::shared_ptr<EqExpr> issmin = std::make_shared < EqExpr>(op, smin);
                if (issmin->isEqual())
                    return terminateStateOnExecError(state, "llvm.abs called with poison and INT_MIN");
            }

            // conditions to flip the sign: INT_MIN < op < 0
            std::shared_ptr<Expr> negative = std::make_shared < SltExpr>(op, zero);
            std::shared_ptr<Expr> notsmin = std::make_shared < NeExpr>(op, smin);
            std::shared_ptr<Expr> cond = std::make_shared < AndExpr>(negative, notsmin);

            // flip and select the result
            std::shared_ptr<Expr> flip = std::make_shared < MulExpr>(op, mone);
            std::shared_ptr<Expr> result = std::make_shared < SelectExpr>(cond, flip, op);

            state->bindLocal(wrapper, result);
            break;
        }

        case llvm::Intrinsic::smax:
        case llvm::Intrinsic::smin:
        case llvm::Intrinsic::umax:
        case llvm::Intrinsic::umin: {
            if (isa<llvm::VectorType>(i->getOperand(0)->getType()) ||
                isa<llvm::VectorType>(i->getOperand(1)->getType()))
                return terminateStateOnExecError(
                    state, "llvm.{s,u}{max,min} with vectors is not supported");

            std::shared_ptr<Expr> op1 = state->eval(wrapper, 1);
            std::shared_ptr<Expr> op2 = state->eval(wrapper, 2);

            std::shared_ptr<Expr> cond = nullptr;
            if (func->getIntrinsicID() == llvm::Intrinsic::smax)
                cond = std::make_shared < SgtExpr>(op1, op2);
            else if (func->getIntrinsicID() == llvm::Intrinsic::smin)
                cond = std::make_shared < SltExpr>(op1, op2);
            else if (func->getIntrinsicID() == llvm::Intrinsic::umax)
                cond = std::make_shared < UgtExpr>(op1, op2);
            else // (func->getIntrinsicID() == Intrinsic::umin)
                cond = std::make_shared < UltExpr>(op1, op2);

            std::shared_ptr<Expr> result = std::make_shared < SelectExpr>(cond, op1, op2);
            state->bindLocal(wrapper, result);
            break;
        }

        case llvm::Intrinsic::fshr:
        case llvm::Intrinsic::fshl: {
            std::shared_ptr<Expr> op1 = state->eval(wrapper, 1);
            std::shared_ptr<Expr> op2 = state->eval(wrapper, 2);
            std::shared_ptr<Expr> op3 = state->eval(wrapper, 3);
            unsigned w = op1->getWidth();
            assert(w == op2->getWidth() && "type mismatch");
            assert(w == op3->getWidth() && "type mismatch");
            std::shared_ptr<Expr> c = std::make_shared < ConcatExpr>(op1, op2);
            // op3 = zeroExtend(op3 % w)
            op3 = std::make_shared < URemExpr>(op3, std::make_shared<ConstantExpr>(w, w));
            op3 = std::make_shared < ZExtExpr>(op3, w + w);
            if (func->getIntrinsicID() == llvm::Intrinsic::fshl) {
                // shift left and take top half
                std::shared_ptr<Expr> s = std::make_shared < ShlExpr>(c, op3);
                state->bindLocal(wrapper, std::make_shared < ExtractExpr>(s, w, w));
            }
            else {
                // shift right and take bottom half
                // note that LShr and AShr will have same behaviour
                std::shared_ptr<Expr> s = std::make_shared < LShrExpr>(c, op3);
                state->bindLocal(wrapper, std::make_shared < ExtractExpr>(s, 0, w));
            }
            break;
        }

                            // va_arg is handled by caller and intrinsic lowering, see comment for
                            // ExecutionState::varargs
        case llvm::Intrinsic::vastart: {
            std::shared_ptr<StackFrame> sf = state->getStackFrame();

            // varargs can be zero if no varargs were provided
            if (!sf->varargs) return;

            // FIXME: This is really specific to the architecture, not the pointer
            // size. This happens to work for x86-32 and x86-64, however.
            uint32_t WordSize = sizeof(void *);
            if (WordSize == 32) {
                executeMemoryOperation(state, true, arguments[0], sf->varargs->getBaseExpr(), 0);
            }
            else {
                assert(WordSize == 64 && "Unknown word size!");

                // x86-64 has quite complicated calling convention. However,
                // instead of implementing it, we can do a simple hack: just
                // make a function believe that all varargs are on stack.
                executeMemoryOperation(
                    state, true,
                    arguments[0],
                    std::make_shared <ConstantExpr>(32,48), 0); // gp_offset
                executeMemoryOperation(
                    state, true,
                    std::make_shared<AddExpr>(arguments[0], std::make_shared <ConstantExpr>(64, 4)),
                    std::make_shared <ConstantExpr>(32,304), 0); // fp_offset
                executeMemoryOperation(
                    state, true,
                    std::make_shared < AddExpr>(arguments[0], std::make_shared <ConstantExpr>(64,8)),
                    sf->varargs->getBaseExpr(), 0); // overflow_arg_area
                executeMemoryOperation(
                    state, true,
                    std::make_shared <AddExpr>(arguments[0], std::make_shared <ConstantExpr>(16, 64)),
                    std::make_shared <ConstantExpr>(64,0), 0); // reg_save_area
            }
            break;
        }

        case llvm::Intrinsic::eh_typeid_for: {
            state->bindLocal(wrapper, getEhTypeidFor(arguments.at(0)));
            break;
        }
        case llvm::Intrinsic::vaend:
            // va_end is a noop for the interpreter.
            //
            // FIXME: We should validate that the target didn't do something bad
            // with va_end, however (like call it twice).
            break;

        case llvm::Intrinsic::vacopy:
            // va_copy should have been lowered.
            //
            // FIXME: It would be nice to check for errors in the usage of this as
            // well.
        default:
            Logger::warn("unimplemented intrinsic: {}", func->getName().data());
            terminateStateOnExecError(state, "unimplemented intrinsic");
            return;
        }

        if (llvm::InvokeInst* ii = llvm::dyn_cast<llvm::InvokeInst>(i)) {
            transferToBasicBlock(ii->getNormalDest(), i->getParent(), state);
        }
    }
    else {
        // Check if maximum stack size was reached.
        // We currently only count the number of stack frames
        if (state->getStack().size() > GlobalSetting::RuntimeMaxStackFrames) {
            terminateStateEarly(state, "Maximum stack size reached.", StateTerminationType::OutOfStackMemory);
            Logger::warn("Maximum stack size reached.");
            return;
        }

        // FIXME: I'm not really happy about this reliance on prevPC but it is ok, I
        // guess. This just done to avoid having to pass KInstIterator everywhere
        // instead of the actual instruction, since we can't make a KInstIterator
        // from just an instruction (unlike LLVM).
        std::shared_ptr<FunctionWrapper> kf = m_moduleStorage->getFunction(func);

        state->pushFrame(state->prevPC, kf);
        state->PC = kf->instructions.begin();

        //if (statsTracker)
        //    statsTracker->framePushed(state, &state.stack[state.stack.size() - 2]);

        // TODO: support zeroext, signext, sret attributes

        uint64_t callingArgs = arguments.size();
        uint64_t funcArgs = func->arg_size();
        if (!func->isVarArg()) {
            if (callingArgs > funcArgs) {
                Logger::warn("calling %s with extra arguments.", func->getName().data());
            }
            else if (callingArgs < funcArgs) {
                terminateStateOnUserError(state, "calling function with too few arguments");
                return;
            }
        }
        else {
            if (callingArgs < funcArgs) {
                terminateStateOnUserError(state, "calling function with too few arguments");
                return;
            }

            uint64_t size = 0; // total size of variadic arguments
            bool requires16ByteAlignment = false;

            std::vector<uint64_t> offsets(callingArgs,0); // offsets of variadic arguments
            uint64_t argWidth;             // width of current variadic argument

            const llvm::CallBase& cb = llvm::cast<llvm::CallBase>(*i);
            for (unsigned k = funcArgs; k < callingArgs; k++) {
                if (cb.isByValArgument(k)) {
                    llvm::Type* t = cb.getParamByValType(k);
                    argWidth = m_moduleStorage->getTypeSizeInBits(t);
                }
                else {
                    argWidth = arguments[k]->getWidth();
                }

                llvm::MaybeAlign ma = cb.getParamAlign(k);
                unsigned alignment = ma ? ma->value() : 0;

                if (sizeof(void*) == 32 && !alignment)
                    alignment = 4;
                else {
                    // AMD64-ABI 3.5.7p5: Step 7. Align l->overflow_arg_area upwards to a
                    // 16 byte boundary if alignment needed by type exceeds 8 byte
                    // boundary.
                    if (!alignment && argWidth > 64) {
                        alignment = 16;
                        requires16ByteAlignment = true;
                    }

                    if (!alignment)
                        alignment = 8;
                }

                size = llvm::alignTo(size, alignment);
                offsets[k] = size;

                if (sizeof(void*) == 32)
                    size += Expr::getMinBytesForWidth(argWidth);
                else {
                    // AMD64-ABI 3.5.7p5: Step 9. Set l->overflow_arg_area to:
                    // l->overflow_arg_area + sizeof(type)
                    // Step 10. Align l->overflow_arg_area upwards to an 8 byte boundary.
                    size += llvm::alignTo(argWidth, sizeof(void*)) / 8;
                }
            }

            std::shared_ptr<StackFrame> sf = state->getStackFrame();
            std::shared_ptr<MemoryObject> mo = MemoryManager::alloc(size);
            mo->isLocal = true;
            //mo->allocSite = (*state->prevPC)->instruction;
            sf->varargs = mo;
            if (!mo && size) {
                terminateStateOnExecError(state, "out of memory (varargs)");
                return;
            }

            if (mo) {
                if ((sizeof(void *) == 64) && (mo->address & 15) &&
                    requires16ByteAlignment) {
                    // Both 64bit Linux/Glibc and 64bit MacOSX should align to 16 bytes.
                    Logger::warn("While allocating varargs: malloc did not align to 16 bytes.");
                }

                //ObjectState* os = bindObjectInState(state, mo, true);

                //for (unsigned k = funcArgs; k < callingArgs; k++) {
                //    if (!cb.isByValArgument(k)) {
                //        os->write(offsets[k], arguments[k]);
                //    }
                //    else {
                //        std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(arguments[k]);
                //        assert(CE); // byval argument needs to be a concrete pointer

                //        ObjectPair op;
                //        state.addressSpace.resolveOne(CE, op);
                //        const ObjectState* osarg = op.second;
                //        assert(osarg);
                //        for (unsigned i = 0; i < osarg->size; i++)
                //            os->write(offsets[k] + i, osarg->read8(i));
                //    }
                //}
            }
        }

        for (unsigned k = 0; k < func->arg_size(); k++)
            state->bindArgument(kf, k, arguments[k]);
    }
}

void Application::executeAlloc(std::shared_ptr<ExecutionState>& state, std::shared_ptr<Expr> size, bool isLocal, std::shared_ptr<InstructionWrapper> target, bool zeroMemory, const std::shared_ptr<ObjectState>& reallocFrom, size_t allocationAlignment)
{
    size = toUnique(state, size);
    //if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(size)) {
    //    const llvm::Value* allocSite = (*state->prevPC)->instruction;
    //    if (allocationAlignment == 0) {
    //        allocationAlignment = getAllocationAlignment(allocSite);
    //    }
    //    MemoryObject* mo =
    //        memory->allocate(CE->getZExtValue(), isLocal, /*isGlobal=*/false,
    //            &state, allocSite, allocationAlignment);
    //    if (!mo) {
    //        state->bindLocal(target,std::make_shared<ConstantExpr>(llvm::APInt(sizeof(void*),0)));
    //    }
    //    else {
    //        ObjectState* os = bindObjectInState(state, mo, isLocal);
    //        if (zeroMemory) {
    //            os->initializeToZero();
    //        }
    //        else {
    //            os->initializeToRandom();
    //        }
    //        bindLocal(target, state, mo->getBaseExpr());

    //        if (reallocFrom) {
    //            unsigned count = std::min(reallocFrom->size, os->size);
    //            for (unsigned i = 0; i < count; i++)
    //                os->write(i, reallocFrom->read8(i));
    //            const MemoryObject* reallocObject = reallocFrom->getObject();
    //            state.deallocate(reallocObject);
    //            state.addressSpace.unbindObject(reallocObject);
    //        }
    //    }
    //}
    //else {
    //    // XXX For now we just pick a size. Ideally we would support
    //    // symbolic sizes fully but even if we don't it would be better to
    //    // "smartly" pick a value, for example we could fork and pick the
    //    // min and max values and perhaps some intermediate (reasonable
    //    // value).
    //    // 
    //    // It would also be nice to recognize the case when size has
    //    // exactly two values and just fork (but we need to get rid of
    //    // return argument first). This shows up in pcre when llvm
    //    // collapses the size expression with a select.

    //    size = optimizer.optimizeExpr(size, true);

    //    // Check if in seed mode, then try to replicate size from a seed
    //    ref<ConstantExpr> example = getValueFromSeeds(state, size);
    //    if (!example) {
    //        bool success = solver->getValue(state.constraints, size, example,
    //            state.queryMetaData);
    //        assert(success && "FIXME: Unhandled solver failure");
    //        (void)success;

    //        // Try and start with a small example.
    //        Expr::Width W = example->getWidth();
    //        while (example->Ugt(ConstantExpr::alloc(128, W))->isTrue()) {
    //            ref<ConstantExpr> tmp = example->LShr(ConstantExpr::alloc(1, W));
    //            bool res;
    //            [[maybe_unused]] bool success =
    //                solver->mayBeTrue(state.constraints, EqExpr::create(tmp, size), res,
    //                    state.queryMetaData);
    //            assert(success && "FIXME: Unhandled solver failure");
    //            if (!res)
    //                break;
    //            example = tmp;
    //        }
    //    }

    //    StatePair fixedSize =
    //        fork(state, EqExpr::create(example, size), true, BranchType::Alloc);

    //    if (fixedSize.second) {
    //        // Check for exactly two values
    //        ref<ConstantExpr> tmp;
    //        bool success = solver->getValue(fixedSize.second->constraints, size, tmp,
    //            fixedSize.second->queryMetaData);
    //        assert(success && "FIXME: Unhandled solver failure");
    //        (void)success;
    //        bool res;
    //        success = solver->mustBeTrue(fixedSize.second->constraints,
    //            EqExpr::create(tmp, size), res,
    //            fixedSize.second->queryMetaData);
    //        assert(success && "FIXME: Unhandled solver failure");
    //        (void)success;
    //        if (res) {
    //            executeAlloc(*fixedSize.second, tmp, isLocal,
    //                target, zeroMemory, reallocFrom);
    //        }
    //        else {
    //            // See if a *really* big value is possible. If so assume
    //            // malloc will fail for it, so lets fork and return 0.
    //            StatePair hugeSize =
    //                fork(*fixedSize.second,
    //                    UltExpr::create(
    //                        ConstantExpr::alloc(1U << 31, example->getWidth()), size),
    //                    true, BranchType::Alloc);
    //            if (hugeSize.first) {
    //                klee_message("NOTE: found huge malloc, returning 0");
    //                bindLocal(target, *hugeSize.first,
    //                    ConstantExpr::alloc(0, Context::get().getPointerWidth()));
    //            }

    //            if (hugeSize.second) {

    //                std::string Str;
    //                llvm::raw_string_ostream info(Str);
    //                ExprPPrinter::printOne(info, "  size expr", size);
    //                info << "  concretization : " << example << "\n";
    //                info << "  unbound example: " << tmp << "\n";
    //                terminateStateOnProgramError(*hugeSize.second,
    //                    "concretized symbolic size",
    //                    StateTerminationType::Model, info.str());
    //            }
    //        }
    //    }

    //    if (fixedSize.first) // can be zero when fork fails
    //        executeAlloc(*fixedSize.first, example, isLocal,
    //            target, zeroMemory, reallocFrom);
    //}
}

std::shared_ptr<InstructionWrapper> Application::getLastNonKleeInternalInstruction(const std::shared_ptr<ExecutionState>& state, std::shared_ptr<InstructionWrapper>& lastInstruction)
{
    const std::vector<std::shared_ptr<StackFrame>>& stack = state->getStack();
    auto it = stack.crbegin(), itE = stack.crend();

    // don't check beyond the outermost function (i.e. main())
    itE--;

    std::shared_ptr<InstructionWrapper> ii;
    if (m_moduleStorage->getInternalFunctions().count((*it)->function->function) == 0) {
        ii = *(state->prevPC);
        lastInstruction = *(state->prevPC);
        //  Cannot return yet because even though
        //  it->function is not an internal function it might of
        //  been called from an internal function.
    }

    // Wind up the stack and check if we are in a KLEE internal function.
    // We visit the entire stack because we want to return a CallInstruction
    // that was not reached via any KLEE internal functions.
    for (; it != itE; ++it) {
        // check calling instruction and if it is contained in a KLEE internal function
        std::shared_ptr<StackFrame> stackFrame = (*it);
        llvm::Function* f = (*stackFrame->caller)->instruction->getParent()->getParent();
        if (m_moduleStorage->getInternalFunctions().count(f)) {
            ii = 0;
            continue;
        }
        if (!ii) {
            ii = *stackFrame->caller;
            lastInstruction = *stackFrame->caller;
        }
    }

    if (!ii) {
        // something went wrong, play safe and return the current instruction info
        lastInstruction = *(state->prevPC);
        return *(state->prevPC);
    }
    return ii;
}

void Application::executeMemoryOperation(std::shared_ptr<ExecutionState>& state, bool isWrite, std::shared_ptr<Expr> address, std::shared_ptr<Expr> value, std::shared_ptr<InstructionWrapper> target)
{
    //uint32_t type = (isWrite ? value->getWidth() : getWidthForLLVMType(target->instruction->getType()));
    //unsigned bytes = Expr::getMinBytesForWidth(type);

    //if (SimplifySymIndices) {
    //    if (!isa<ConstantExpr>(address))
    //        address = ConstraintManager::simplifyExpr(state.constraints, address);
    //    if (isWrite && !isa<ConstantExpr>(value))
    //        value = ConstraintManager::simplifyExpr(state.constraints, value);
    //}

    //address = optimizer.optimizeExpr(address, true);

    //ObjectPair op;
    //bool success;
    //solver->setTimeout(coreSolverTimeout);

    //bool resolveSingleObject = SingleObjectResolution;

    //if (resolveSingleObject && !isa<ConstantExpr>(address)) {
    //    // Address is symbolic

    //    resolveSingleObject = false;
    //    auto base_it = state.base_addrs.find(address);
    //    if (base_it != state.base_addrs.end()) {
    //        // Concrete address found in the map, now find the associated memory
    //        // object
    //        if (!state.addressSpace.resolveOne(state, solver.get(), base_it->second, op,
    //            success) ||
    //            !success) {
    //            klee_warning("Failed to resolve concrete address from the base_addrs "
    //                "map to a memory object");
    //        }
    //        else {
    //            // We have resolved the stored concrete address to a memory object.
    //            // Now let's see if we can prove an overflow - we are only interested in
    //            // two cases: either we overflow and it's a bug or we don't and we carry
    //            // on; in this mode we are not interested in trying out other memory
    //            // objects
    //            resolveSingleObject = true;
    //        }
    //    }
    //}
    //else {
    //    resolveSingleObject = false;
    //}

    //if (!resolveSingleObject) {
    //    if (!state.addressSpace.resolveOne(state, solver.get(), address, op, success)) {
    //        address = toConstant(state, address, "resolveOne failure");
    //        success = state.addressSpace.resolveOne(cast<ConstantExpr>(address), op);
    //    }
    //    solver->setTimeout(time::Span());

    //    if (success) {
    //        const MemoryObject* mo = op.first;

    //        if (MaxSymArraySize && mo->size >= MaxSymArraySize) {
    //            address = toConstant(state, address, "max-sym-array-size");
    //        }

    //        ref<Expr> offset = mo->getOffsetExpr(address);
    //        ref<Expr> check = mo->getBoundsCheckOffset(offset, bytes);
    //        check = optimizer.optimizeExpr(check, true);

    //        bool inBounds;
    //        solver->setTimeout(coreSolverTimeout);
    //        bool success = solver->mustBeTrue(state.constraints, check, inBounds,
    //            state.queryMetaData);
    //        solver->setTimeout(time::Span());
    //        if (!success) {
    //            state.pc = state.prevPC;
    //            terminateStateOnSolverError(state, "Query timed out (bounds check).");
    //            return;
    //        }

    //        if (inBounds) {
    //            const ObjectState* os = op.second;
    //            if (isWrite) {
    //                if (os->readOnly) {
    //                    terminateStateOnProgramError(state, "memory error: object read only",
    //                        StateTerminationType::ReadOnly);
    //                }
    //                else {
    //                    ObjectState* wos = state.addressSpace.getWriteable(mo, os);
    //                    wos->write(offset, value);
    //                }
    //            }
    //            else {
    //                ref<Expr> result = os->read(offset, type);

    //                if (interpreterOpts.MakeConcreteSymbolic)
    //                    result = replaceReadWithSymbolic(state, result);

    //                bindLocal(target, state, result);
    //            }

    //            return;
    //        }
    //    }
    //}

    //// we are on an error path (no resolution, multiple resolution, one
    //// resolution with out of bounds), or we do a single object resolution

    //address = optimizer.optimizeExpr(address, true);
    //ResolutionList rl;
    //bool incomplete = false;

    //if (!resolveSingleObject) {
    //    solver->setTimeout(coreSolverTimeout);
    //    incomplete = state.addressSpace.resolve(state, solver.get(), address, rl, 0,
    //        coreSolverTimeout);
    //    solver->setTimeout(time::Span());
    //}
    //else {
    //    rl.push_back(op); // we already have the object pair, no need to look for it
    //}
    //// XXX there is some query wasteage here. who cares?
    //ExecutionState* unbound = &state;

    //for (ResolutionList::iterator i = rl.begin(), ie = rl.end(); i != ie; ++i) {
    //    const MemoryObject* mo = i->first;
    //    const ObjectState* os = i->second;
    //    ref<Expr> inBounds = mo->getBoundsCheckPointer(address, bytes);

    //    StatePair branches = fork(*unbound, inBounds, true, BranchType::MemOp);
    //    ExecutionState* bound = branches.first;

    //    // bound can be 0 on failure or overlapped 
    //    if (bound) {
    //        if (isWrite) {
    //            if (os->readOnly) {
    //                terminateStateOnProgramError(*bound, "memory error: object read only",
    //                    StateTerminationType::ReadOnly);
    //            }
    //            else {
    //                ObjectState* wos = bound->addressSpace.getWriteable(mo, os);
    //                wos->write(mo->getOffsetExpr(address), value);
    //            }
    //        }
    //        else {
    //            ref<Expr> result = os->read(mo->getOffsetExpr(address), type);
    //            bindLocal(target, *bound, result);
    //        }
    //    }

    //    unbound = branches.second;
    //    if (!unbound)
    //        break;
    //}

    //// XXX should we distinguish out of bounds and overlapped cases?
    //if (unbound) {
    //    if (incomplete) {
    //        terminateStateOnSolverError(*unbound, "Query timed out (resolve).");
    //    }
    //    else {
    //        if (auto CE = dyn_cast<ConstantExpr>(address)) {
    //            std::uintptr_t ptrval = CE->getZExtValue();
    //            auto ptr = reinterpret_cast<void*>(ptrval);
    //            if (ptrval < MemoryManager::pageSize) {
    //                terminateStateOnProgramError(
    //                    *unbound, "memory error: null page access",
    //                    StateTerminationType::Ptr, getAddressInfo(*unbound, address));
    //                return;
    //            }
    //            else if (MemoryManager::isDeterministic) {
    //                using kdalloc::LocationInfo;
    //                auto li = unbound->heapAllocator.locationInfo(ptr, bytes);
    //                if (li == LocationInfo::LI_AllocatedOrQuarantined) {
    //                    // In case there is no size mismatch (checked by resolving for base
    //                    // address), the object is quarantined.
    //                    auto base = reinterpret_cast<std::uintptr_t>(li.getBaseAddress());
    //                    auto baseExpr = Expr::createPointer(base);
    //                    ObjectPair op;
    //                    if (!unbound->addressSpace.resolveOne(baseExpr, op)) {
    //                        terminateStateOnProgramError(
    //                            *unbound, "memory error: use after free",
    //                            StateTerminationType::Ptr, getAddressInfo(*unbound, address));
    //                        return;
    //                    }
    //                }
    //            }
    //        }
    //        terminateStateOnProgramError(
    //            *unbound, "memory error: out of bound pointer",
    //            StateTerminationType::Ptr, getAddressInfo(*unbound, address));
    //    }
    //}
}

void Application::processTestCase(const std::shared_ptr<ExecutionState>& state, const std::string& message)
{
    std::vector< std::pair<std::string, std::vector<unsigned char> > > out;
    //bool success = getSymbolicSolution(state, out);

    //if (!success)  Logger::warn("unable to get symbolic solution, losing test case");

    //const auto start_time = time::getWallTime();

    //unsigned id = ++m_numTotalTests;

    //if (success) {
    //    KTest b;
    //    b.numArgs = m_argc;
    //    b.args = m_argv;
    //    b.symArgvs = 0;
    //    b.symArgvLen = 0;
    //    b.numObjects = out.size();
    //    b.objects = new KTestObject[b.numObjects];
    //    assert(b.objects);
    //    for (unsigned i = 0; i < b.numObjects; i++) {
    //        KTestObject* o = &b.objects[i];
    //        o->name = const_cast<char*>(out[i].first.c_str());
    //        o->numBytes = out[i].second.size();
    //        o->bytes = new unsigned char[o->numBytes];
    //        assert(o->bytes);
    //        std::copy(out[i].second.begin(), out[i].second.end(), o->bytes);
    //    }

    //    if (!kTest_toFile(&b, getOutputFilename(getTestFilename("ktest", id)).c_str())) {
    //        klee_warning("unable to write output test case, losing it");
    //    }
    //    else {
    //        ++m_numGeneratedTests;
    //    }

    //    for (unsigned i = 0; i < b.numObjects; i++)
    //        delete[] b.objects[i].bytes;
    //    delete[] b.objects;
    //}

    //if (message.empty() == false) {
    //    auto f = openTestFile(errorSuffix, id);
    //    if (f)
    //        *f << errorMessage;
    //}

    //if (m_pathWriter) {
    //    std::vector<unsigned char> concreteBranches;
    //    m_pathWriter->readStream(m_interpreter->getPathStreamID(state),
    //        concreteBranches);
    //    auto f = openTestFile("path", id);
    //    if (f) {
    //        for (const auto& branch : concreteBranches) {
    //            *f << branch << '\n';
    //        }
    //    }
    //}

    //if (message.empty() == false) {
    //    std::string constraints;
    //    m_interpreter->getConstraintLog(state, constraints, Interpreter::KQUERY);
    //    auto f = openTestFile("kquery", id);
    //    if (f)
    //        *f << constraints;
    //}

    //if (WriteCVCs) {
    //    // FIXME: If using Z3 as the core solver the emitted file is actually
    //    // SMT-LIBv2 not CVC which is a bit confusing
    //    std::string constraints;
    //    m_interpreter->getConstraintLog(state, constraints, Interpreter::STP);
    //    auto f = openTestFile("cvc", id);
    //    if (f)
    //        *f << constraints;
    //}

    //if (WriteSMT2s) {
    //    std::string constraints;
    //    m_interpreter->getConstraintLog(state, constraints, Interpreter::SMTLIB2);
    //    auto f = openTestFile("smt2", id);
    //    if (f)
    //        *f << constraints;
    //}

    //if (m_symPathWriter) {
    //    std::vector<unsigned char> symbolicBranches;
    //    m_symPathWriter->readStream(m_interpreter->getSymbolicPathStreamID(state),
    //        symbolicBranches);
    //    auto f = openTestFile("sym.path", id);
    //    if (f) {
    //        for (const auto& branch : symbolicBranches) {
    //            *f << branch << '\n';
    //        }
    //    }
    //}

    //if (WriteCov) {
    //    std::map<const std::string*, std::set<unsigned> > cov;
    //    m_interpreter->getCoveredLines(state, cov);
    //    auto f = openTestFile("cov", id);
    //    if (f) {
    //        for (const auto& entry : cov) {
    //            for (const auto& line : entry.second) {
    //                *f << *entry.first << ':' << line << '\n';
    //            }
    //        }
    //    }
    //}

    //time::Span elapsed_time(time::getWallTime() - start_time);
    //auto f = openTestFile("info", id);
    //if (f)
    //    *f << "Time to generate test case: " << elapsed_time << '\n';

    //if (message.empty() == false) {
    //    m_interpreter->prepareForEarlyExit();
    //    Logger::warn("EXITING ON ERROR:\n%s\n", message);
    //}
}

std::shared_ptr<ConstantExpr> Application::getEhTypeidFor(std::shared_ptr<Expr> typeInfo)
{
    auto eh_type_iterator = std::find(std::begin(m_eh_typeids), std::end(m_eh_typeids), typeInfo);
    if (eh_type_iterator == std::end(m_eh_typeids)) {
        m_eh_typeids.push_back(typeInfo);
        eh_type_iterator = std::prev(std::end(m_eh_typeids));
    }
    // +1 because typeids must always be positive, so they can be distinguished
    // from 'no landingpad clause matched' which has value 0
    llvm::APInt value(32, eh_type_iterator - std::begin(m_eh_typeids) + 1);
    auto res = std::make_shared<ConstantExpr>(value);
    return res;
}

void Application::terminateState(const std::shared_ptr<ExecutionState>& state, StateTerminationType reason)
{
    //if (replayKTest && replayPosition != replayKTest->numObjects) {
    //    klee_warning_once(replayKTest,
    //        "replay did not consume all objects in test input.");
    //}

    //interpreterHandler->incPathsExplored();
    //executionTree->setTerminationType(state, reason);

    //std::vector<ExecutionState*>::iterator it =
    //    std::find(addedStates.begin(), addedStates.end(), &state);
    //if (it == addedStates.end()) {
    //    state.pc = state.prevPC;

    //    removedStates.push_back(&state);
    //}
    //else {
    //    // never reached searcher, just delete immediately
    //    std::map< ExecutionState*, std::vector<SeedInfo> >::iterator it3 =
    //        seedMap.find(&state);
    //    if (it3 != seedMap.end())
    //        seedMap.erase(it3);
    //    addedStates.erase(it);
    //    executionTree->remove(state.executionTreeNode);
    //    delete& state;
    //}
}

void Application::terminateStateOnUserError(std::shared_ptr<ExecutionState>& state, const std::string& message, bool writeErr)
{
    ++StatisticManager::terminationUserError;
    if (writeErr) {
        terminateStateOnError(state, message, StateTerminationType::User);
    }
    else {
        terminateState(state, StateTerminationType::User);
    }
}

void Application::terminateStateEarly(std::shared_ptr<ExecutionState>& state, const std::string& message, StateTerminationType reason)
{
    if (reason <= StateTerminationType::OutOfStackMemory && reason > StateTerminationType::Exit) {
        StatisticManager::terminationEarly++;
        processTestCase(state, (message + "\n").c_str());
    }

    terminateState(state, reason);
}

bool Application::shouldExitOn(StateTerminationType reason)
{
    switch (reason)
    {
    case Abort:
    case Assert:
    case BadVectorAccess:
    case Execution:
    case External:
    case Free:
    case Model:
    case Overflow:
    case Ptr:
    case ReadOnly:
    case ReportError:
    case InvalidBuiltin:
    case ImplicitTruncation:
    case UnreachableCall:
    case MissingReturn:
    case InvalidLoad:
    case NullableAttribute:
    case User:
        return true;
    }
    return false;
}

void Application::callExternalFunction(std::shared_ptr<ExecutionState>& state, std::shared_ptr<InstructionWrapper> target, std::shared_ptr<FunctionWrapper> &callable, std::vector<std::shared_ptr<Expr>>& arguments)
{
//    if (const auto* func = dyn_cast<KFunction>(callable);
//        func &&
//        specialFunctionHandler->handle(state, func->function, target, arguments))
//        return;
//
//    if (ExternalCalls == ExternalCallPolicy::None &&
//        !okExternals.count(callable->getName().str())) {
//        klee_warning("Disallowed call to external function: %s\n",
//            callable->getName().str().c_str());
//        terminateStateOnUserError(state, "external calls disallowed");
//        return;
//    }
//
//    // normal external function handling path
//    // allocate 512 bits for each argument (+return value) to support
//    // fp80's and SIMD vectors as parameters for external calls;
//    // we could iterate through all the arguments first and determine the exact
//    // size we need, but this is faster, and the memory usage isn't significant.
//    size_t allocatedBytes = Expr::MaxWidth / 8 * (arguments.size() + 1);
//    uint64_t* args = (uint64_t*)alloca(allocatedBytes);
//    memset(args, 0, allocatedBytes);
//    unsigned wordIndex = 2;
//    for (auto& a : arguments) {
//        if (ExternalCalls == ExternalCallPolicy::All ||
//            ExternalCalls == ExternalCallPolicy::OverApprox) {
//            a = optimizer.optimizeExpr(a, true);
//            ref<ConstantExpr> cvalue = toConstant(
//                state, a, "external call", ExternalCalls == ExternalCallPolicy::All);
//            cvalue->toMemory(&args[wordIndex]);
//
//            // If the argument points to a valid and writable object, concretise it
//            // according to the selected policy
//            if (ObjectPair op;
//                cvalue->getWidth() == Context::get().getPointerWidth() &&
//                state.addressSpace.resolveOne(cvalue, op) && !op.second->readOnly) {
//                auto* os = state.addressSpace.getWriteable(op.first, op.second);
//                os->flushToConcreteStore(*this, state,
//                    ExternalCalls == ExternalCallPolicy::All);
//            }
//
//            wordIndex += (cvalue->getWidth() + 63) / 64;
//        }
//        else {
//            ref<Expr> arg = toUnique(state, a);
//            if (ConstantExpr* ce = dyn_cast<ConstantExpr>(arg)) {
//                // fp80 must be aligned to 16 according to the System V AMD 64 ABI
//                if (ce->getWidth() == Expr::Fl80 && wordIndex & 0x01)
//                    wordIndex++;
//
//                // XXX kick toMemory functions from here
//                ce->toMemory(&args[wordIndex]);
//                wordIndex += (ce->getWidth() + 63) / 64;
//            }
//            else {
//                terminateStateOnExecError(state,
//                    "external call with symbolic argument: " +
//                    callable->getName());
//                return;
//            }
//        }
//    }
//
//    // Prepare external memory for invoking the function
//    static std::size_t residentPages = 0;
//    double avgNeededPages = 0;
//    if (MemoryManager::isDeterministic) {
//        auto const minflt = [] {
//            struct rusage ru = {};
//            [[maybe_unused]] int ret = getrusage(RUSAGE_SELF, &ru);
//            assert(!ret && "getrusage failed");
//            assert(ru.ru_minflt >= 0);
//            return ru.ru_minflt;
//            };
//
//        auto tmp = minflt();
//        std::size_t neededPages = state.addressSpace.copyOutConcretes();
//        auto newPages = minflt() - tmp;
//        assert(newPages >= 0);
//        residentPages += newPages;
//        assert(residentPages >= neededPages &&
//            "allocator too full, assumption that each object occupies its own "
//            "page is no longer true");
//
//        // average of pages needed for an external function call
//        static double avgNeededPages_ = residentPages;
//        // exponential moving average with alpha = 1/3
//        avgNeededPages_ = (3.0 * avgNeededPages_ + neededPages) / 4.0;
//        avgNeededPages = avgNeededPages_;
//    }
//    else {
//        state.addressSpace.copyOutConcretes();
//    }
//
//#ifndef WINDOWS
//    // Update external errno state with local state value
//    int* errno_addr = getErrnoLocation(state);
//    ObjectPair result;
//    bool resolved = state.addressSpace.resolveOne(
//        ConstantExpr::create((uint64_t)errno_addr, Expr::Int64), result);
//    if (!resolved)
//        klee_error("Could not resolve memory object for errno");
//    ref<Expr> errValueExpr = result.second->read(0, sizeof(*errno_addr) * 8);
//    ConstantExpr* errnoValue = dyn_cast<ConstantExpr>(errValueExpr);
//    if (!errnoValue) {
//        terminateStateOnExecError(state,
//            "external call with errno value symbolic: " +
//            callable->getName());
//        return;
//    }
//
//    externalDispatcher->setLastErrno(
//        errnoValue->getZExtValue(sizeof(*errno_addr) * 8));
//#endif
//
//    if (ExternalCallWarnings != ExtCallWarnings::None) {
//        std::string TmpStr;
//        llvm::raw_string_ostream os(TmpStr);
//        os << "calling external: " << callable->getName().str() << "(";
//        for (unsigned i = 0; i < arguments.size(); i++) {
//            os << arguments[i];
//            if (i != arguments.size() - 1)
//                os << ", ";
//        }
//        os << ") at " << state.pc->getSourceLocation();
//
//        if (ExternalCallWarnings == ExtCallWarnings::All)
//            klee_warning("%s", os.str().c_str());
//        else
//            klee_warning_once(callable->getValue(), "%s", os.str().c_str());
//    }
//
//    bool success = externalDispatcher->executeCall(callable, target->inst, args);
//    if (!success) {
//        terminateStateOnExecError(state,
//            "failed external call: " + callable->getName(),
//            StateTerminationType::External);
//        return;
//    }
//
//    if (!state.addressSpace.copyInConcretes(ExternalCalls ==
//        ExternalCallPolicy::All)) {
//        terminateStateOnExecError(state, "external modified read-only object",
//            StateTerminationType::External);
//        return;
//    }
//
//    if (MemoryManager::isDeterministic && residentPages > ExternalPageThreshold &&
//        residentPages > 2 * avgNeededPages) {
//        if (memory->markMappingsAsUnneeded()) {
//            residentPages = 0;
//        }
//    }
//
//#ifndef WINDOWS
//    // Update errno memory object with the errno value from the call
//    int error = externalDispatcher->getLastErrno();
//    state.addressSpace.copyInConcrete(result.first, result.second,
//        (uint64_t)&error,
//        ExternalCalls == ExternalCallPolicy::All);
//#endif
//
//    Type* resultType = target->inst->getType();
//    if (resultType != Type::getVoidTy(kmodule->module->getContext())) {
//        ref<Expr> e =
//            ConstantExpr::fromMemory((void*)args, getWidthForLLVMType(resultType));
//        bindLocal(target, state, e);
//    }
}

void Application::exit() {
	llvm::llvm_shutdown();
}
