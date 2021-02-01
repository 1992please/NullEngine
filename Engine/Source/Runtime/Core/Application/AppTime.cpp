#include "AppTime.h"
#include <windows.h>

double FAppTime::TimeSeconds = 0.0;
double FAppTime::DeltaTime = 1 /30.0;
double FAppTime::SecondsPerCycle = 0.0;

void FAppTime::InitAppTime()
{
	LARGE_INTEGER CyclesPerSecond;
	QueryPerformanceFrequency(&CyclesPerSecond);
	SecondsPerCycle = 1.0 / CyclesPerSecond.QuadPart;

	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);
	TimeSeconds = Cycles.QuadPart * SecondsPerCycle;
	DeltaTime = 0.0;
}

void FAppTime::UpdateAppTime()
{
	LARGE_INTEGER Cycles;
	QueryPerformanceCounter(&Cycles);
	double CurrentTime = Cycles.QuadPart * SecondsPerCycle;
	DeltaTime = CurrentTime - TimeSeconds;
	TimeSeconds = CurrentTime;
}
