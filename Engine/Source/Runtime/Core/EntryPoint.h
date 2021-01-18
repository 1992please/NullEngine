#pragma once

extern Application* CreateApplication();

int main(int argc, char** argv)
{
	FLogger::Init();
	Application* App = CreateApplication();
	App->Run();
	delete App;
}