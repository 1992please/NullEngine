#pragma once

extern FApplication* CreateApplication();

int main(int argc, char** argv)
{
	FLogger::Init();
	FApplication* App = CreateApplication();
	App->Run();
	delete App;
}