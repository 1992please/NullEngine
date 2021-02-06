#pragma once
#include "Core/CoreTypes.h"
#include "Core/Logging/Logger.h"
#include "Core/Application/AppTime.h"


struct ProfileResult
{
	const char* Name;

	double Start;
	double ElapsedTime;
};

class FProfiler
{
};


class FProfilingTimer
{
public:
	FProfilingTimer(const char* InName)
		: Name(InName)
	{
		StartTime = FAppTime::GetTimeNow();
	}
	~FProfilingTimer()
	{
		const double EndTime = FAppTime::GetTimeNow();
		const double ElapsedTime = EndTime - StartTime;

		NE_CORE_LOG("Profiler: %d %s", (uint32)(ElapsedTime * 1000000), Name);
	}
private:
	const char* Name;
	double StartTime;
};