#pragma once
#include "CoreTypes.h"

class NE_API Application
{
public:
	Application();
	virtual ~Application();

	virtual void Run();
};

// should be define in client
Application* CreateApplication();