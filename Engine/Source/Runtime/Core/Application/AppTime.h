#pragma once
#include "Core/CoreTypes.h"

class FAppTime
{
public:
	static void InitAppTime();
	static void UpdateAppTime();
	
	FORCEINLINE static double GetTimeSeconds() { return TimeSeconds; }
	FORCEINLINE static double GetDeltaTime() { return DeltaTime; }
private:
    static double TimeSeconds;
    static double DeltaTime;
	static double SecondsPerCycle;
};