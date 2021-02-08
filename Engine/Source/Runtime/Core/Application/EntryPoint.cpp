#include "NullPCH.h"
#include "Core/CoreTypes.h"
#include "Core/Application/Application.h"
#include "Core/Application/AppTime.h"

int main(int argc, char** argv)
{
	NE_PROFILE_INIT()
	FAppTime::InitAppTime();
	FLogger::Init();

	FApplication* App = new FApplication();
	App->Run();
	delete App;
	NE_PROFILE_FINISH()
}