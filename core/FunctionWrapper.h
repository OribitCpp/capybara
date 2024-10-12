#ifndef  FUNCTIONWRAPPER_H
#define FUNCTIONWRAPPER_H

class FunctionWrapper {
public:
	FunctionWrapper();
	~FunctionWrapper();

	FunctionWrapper(const FunctionWrapper&) = delete;
	FunctionWrapper& operator=(const FunctionWrapper&) = delete;
};

#endif // ! FUNCTION?_H
