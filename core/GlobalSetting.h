#ifndef  GLOBALSETTING_H
#define GLOBALSETTING_H
#include <cstdint>

struct GlobalSetting
{
	static uint32_t RuntimeMaxStackFrames;
	static uint32_t SolverMaxLimitTime; // seconds
};

#endif // ! GlobalSetting_h
