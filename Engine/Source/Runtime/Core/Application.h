#pragma once
#include "Core/CoreTypes.h"
#include "Core/Window.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Rendering/GraphicLayers/GraphicLayersStack.h"

class NE_API FApplication
{
public:
	FApplication();
	virtual ~FApplication();

	virtual void Run();

	void OnEvent(IEvent& InEvent);
	void OnWindowCloseEvent(FWindowCloseEvent&);
	FORCEINLINE FWindow* GetWindow() const { return pWindow; }
	FORCEINLINE static FApplication* GetApplication() { return Instance; }
private:
	FWindow* pWindow;
	bool bRunning;
	FGraphicLayersStack GraphicLayerStack;
	static FApplication* Instance;
};

// should be define in client
FApplication* CreateApplication();