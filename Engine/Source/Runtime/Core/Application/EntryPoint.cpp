#pragma once
#include "Core/CoreTypes.h"
#include "Core/Application/Application.h"
#include "Core/Application/AppTime.h"
#include "Core/Misc/CFile.h"

int main(int argc, char** argv)
{
	FLogger::Init();
	FAppTime::InitAppTime();

	FApplication* App = new FApplication();
	App->Run();
	delete App;
}