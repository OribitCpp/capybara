#include "StatisticManager.h"

StatisticManager::~StatisticManager()
{
}

Statistics StatisticManager::terminateOnExit("terminateOnExit");
Statistics StatisticManager::terminationExecutionError("terminationExecutionError");
Statistics StatisticManager::terminationUserError("terminationUserError");
Statistics StatisticManager::terminationEarly("terminationEarly");
Statistics StatisticManager::terminationSolverError("terminationSolverError");

Statistics StatisticManager::queries("queries");
Statistics StatisticManager::forks("forks");
Statistics StatisticManager::inhibitedForks("inhibitedForks");