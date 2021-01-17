#pragma once

extern Application* CreateApplication();

int main(int argc, char** argv)
{
	NullLogger::Init();
	Application* App = CreateApplication();
	App->Run();
	delete App;
}