#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/GraphicLayers/GraphicLayersStack.h"

class FApplication
{
public:
	FApplication();
	virtual ~FApplication();

	virtual void Run();

	void OnEvent(class IEvent& InEvent);
	void OnWindowCloseEvent(class FWindowCloseEvent&);
	FORCEINLINE class IApplicationWindow* GetWindow() const { return pWindow; }
	FORCEINLINE static FApplication* GetApplication() { return Instance; }
private:
	bool bRunning;
	FGraphicLayersStack GraphicLayerStack;

	class IApplicationWindow* pWindow;
	static FApplication* Instance;
	class FImGuiLayer* ImGuiLayer;

};

extern FGraphicLayer* GetExternalLayer();