#pragma once

int main(int argc, char** argv)
{
	FLogger::Init();
	FApplication* App = new FApplication();
	App->Run();
	delete App;
}