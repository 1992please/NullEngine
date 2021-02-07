#pragma once
#include "Core/CoreTypes.h"
#include "Core/Application/Application.h"
#include "Core/Application/AppTime.h"
#include "Core/Misc/ProfilingStats.h"

int main(int argc, char** argv)
{
	NE_PROFILE_INIT()
	FAppTime::InitAppTime();
	FLogger::Init();

	FProfiler* Profiler = FProfiler::Get();
	FApplication* App = new FApplication();
	App->Run();
	delete App;
	NE_PROFILE_FINISH()
}