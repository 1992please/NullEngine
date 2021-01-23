#pragma once
#include "Core/CoreTypes.h"
#include "Core/Window.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/GraphicLayers/GraphicLayersStack.h"

class NE_API FApplication
{
public:
	FApplication();
	virtual ~FApplication();

	virtual void Run();

	void OnEvent(IEvent& InEvent);
	void OnWindowCloseEvent(FWindowCloseEvent&);
private:
	FWindow* pWindow;
	bool bRunning;
	FGraphicLayersStack GraphicLayerStack;
};

// should be define in client
FApplication* CreateApplication();