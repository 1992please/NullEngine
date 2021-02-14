#include "NullPCH.h"
#include "Core/CoreTypes.h"
#include "Core/Application/Application.h"
#include "Core/Application/AppTime.h"

extern FApplication* CreateApplication();

int main(int argc, char** argv)
{
	NE_PROFILE_INIT()
	FAppTime::InitAppTime();
	FLogger::Init();

	FApplication* App = CreateApplication();
	App->Run();
	delete App;
	NE_PROFILE_FINISH()
}