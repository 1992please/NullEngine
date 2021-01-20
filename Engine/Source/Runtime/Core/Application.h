#pragma once
#include "CoreTypes.h"
#include "Window.h"

class NE_API FApplication
{
public:
	FApplication();
	virtual ~FApplication();

	virtual void Run();
private:
	FWindow* pWindow;
	bool bRunning;
};

// should be define in client
FApplication* CreateApplication();