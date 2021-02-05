#pragma once
#include "Core/CoreTypes.h"
#include "Core/Application/Application.h"

int main(int argc, char** argv)
{
	FLogger::Init();
	FApplication* App = new FApplication();
	App->Run();
	delete App;
}