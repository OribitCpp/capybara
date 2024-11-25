#ifndef  StatisticManager_h
#define StatisticManager_h

#include "statistic/Statistics.h"

class StatisticManager {
public:
	~StatisticManager();

	static Statistics terminateOnExit;
	static Statistics terminationExecutionError;
	static Statistics terminationUserError;
	static Statistics terminationEarly;
private:
	StatisticManager();
};

#endif // ! StatisticManager_h
