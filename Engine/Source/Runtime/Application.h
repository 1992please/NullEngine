#pragma once
#include "Core.h"

class NE_API Application
{
public:
	Application();
	virtual ~Application();

	void Run();
};

// should be define in client
Application* CreateApplication();