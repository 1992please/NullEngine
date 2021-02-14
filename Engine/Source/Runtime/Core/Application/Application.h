#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/GraphicLayers/GraphicLayersStack.h"

class FApplication
{
public:
	FApplication(const char* AppName = "Null Engine");
	virtual ~FApplication();

	virtual void Run();

	void PushLayer(FGraphicLayer* InLayer) { GraphicLayerStack.PushLayer(InLayer); }

	void OnEvent(class IEvent& InEvent);
	bool OnWindowCloseEvent(class FWindowCloseEvent& InWindowEvent);
	bool OnWindowResizeEvent(class FWindowResizeEvent& InWindowResizeEvent);
	FORCEINLINE class IApplicationWindow* GetWindow() const { return pWindow; }
	FORCEINLINE static FApplication* GetApplication() { return Instance; }
private:
	bool bRunning;
	bool bMinimized;
	FGraphicLayersStack GraphicLayerStack;

	class IApplicationWindow* pWindow;
	static FApplication* Instance;
	class FImGuiLayer* ImGuiLayer;

};