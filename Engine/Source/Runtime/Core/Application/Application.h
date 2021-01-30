#pragma once
#include "Core/CoreTypes.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Renderer/GraphicLayers/GraphicLayersStack.h"
#include "Renderer/Components/RendererCamera.h"
class FApplication
{
public:
	FApplication();
	virtual ~FApplication();

	virtual void Run();

	void OnEvent(IEvent& InEvent);
	void OnWindowCloseEvent(FWindowCloseEvent&);
	FORCEINLINE class IApplicationWindow* GetWindow() const { return pWindow; }
	FORCEINLINE static FApplication* GetApplication() { return Instance; }
private:
	class IApplicationWindow* pWindow;
	bool bRunning;
	FGraphicLayersStack GraphicLayerStack;
	static FApplication* Instance;
	class FImGuiLayer* ImGuiLayer;
	class IShader* Shader;
	class IShader* Shader2;
	class IVertexArray* VertexArray;
	class IVertexArray* VertexArray2;
	FOrthographicCamera Camera;
};

// should be define in client
FApplication* CreateApplication();