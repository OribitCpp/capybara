#include "StatisticManager.h"

StatisticManager::~StatisticManager()
{
}

Statistics StatisticManager::terminateOnExit("terminateOnExit");
Statistics StatisticManager::terminationExecutionError("terminationExecutionError");
Statistics StatisticManager::terminationUserError("terminationUserError");
Statistics StatisticManager::terminationEarly("terminationEarly");