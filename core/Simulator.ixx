export module Simulator;

class FunctionWrapper;

export class Simulator {
public:
	Simulator();
	~Simulator();

	Simulator(const Simulator&) = delete;
	Simulator& operator=(const Simulator&) = delete;

	void runMainFunction(FunctionWrapper *function);
};