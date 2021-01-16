#pragma once

extern Application* CreateApplication();

int main(int argc, char** argv)
{
	Log::Init();
	NE_CORE_ERROR("Hello Bitch");
	NE_CORE_WARN("Hello Bitch = {1}{0}", "WTF", 5);
	NE_CORE_LOG("Hello Bitch");
	Application* App = CreateApplication();
	App->Run();
	delete App;
}