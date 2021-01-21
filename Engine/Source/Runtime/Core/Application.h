#pragma once
#include "CoreTypes.h"
#include "Window.h"
#include "Events/Event.h"


class NE_API FApplication
{
public:
	FApplication();
	virtual ~FApplication();

	virtual void Run();

	void OnEvent(FEvent& InEvent);
private:
	FWindow* pWindow;
	bool bRunning;
};

// should be define in client
FApplication* CreateApplication();