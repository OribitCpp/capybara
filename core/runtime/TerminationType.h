#ifndef  TERMINATIONTYPES_H
#define TERMINATIONTYPES_H

#include <cstdint>

/// @brief Reason an ExecutionState got terminated.
enum StateTerminationType : std::uint8_t {
	RUNNING = 0U,
	Exit = 1U,
	Interrupted = 10U,
	MaxDepth = 11U,
	OutOfMemory = 12U,
	OutOfStackMemory = 13U,
	Solver = 20U,
	Abort = 30U,
	Assert = 31U,
	BadVectorAccess = 32U,
	Free = 33U,
	Model = 34U,
	Overflow = 35U,
	Ptr = 36U,
	ReadOnly = 37U,
	ReportError = 38U,
	InvalidBuiltin = 39U,
	ImplicitTruncation = 40U,
	ImplicitConversion = 41U,
	UnreachableCall = 42U,
	MissingReturn = 43U,
	InvalidLoad = 44U,
	NullableAttribute = 45U,
	User = 50U,
	Execution = 60U,
	External = 61U,
	Replay = 70U,
	Merge = 71U,
	SilentExit = 80U
};

#endif // ! TERMINATIONTYPES_H
