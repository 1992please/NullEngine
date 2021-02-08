#pragma once
#include "Core/CoreTypes.h"

class FAppTime
{
public:
	FORCEINLINE static void InitAppTime()
	{
		LARGE_INTEGER CyclesPerSecond;
		QueryPerformanceFrequency(&CyclesPerSecond);
		SecondsPerCycle = 1.0 / CyclesPerSecond.QuadPart;

		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);
		TimeSeconds = Cycles.QuadPart * SecondsPerCycle;
		DeltaTime = 0.0;
	}

	FORCEINLINE static double GetTimeNow()
	{
		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);
		return Cycles.QuadPart * SecondsPerCycle;
	}


	FORCEINLINE static void UpdateTime()
	{
		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);
		double CurrentTime = Cycles.QuadPart * SecondsPerCycle;
		DeltaTime = CurrentTime - TimeSeconds;
		TimeSeconds = CurrentTime;
	}

	FORCEINLINE static double GetTimeSeconds() { return TimeSeconds; }
	FORCEINLINE static double GetDeltaTime() { return DeltaTime; }
private:
    static double TimeSeconds;
    static double DeltaTime;
	static double SecondsPerCycle;
};