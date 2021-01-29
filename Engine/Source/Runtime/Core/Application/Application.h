#pragma once
#include "Core/CoreTypes.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"
#include "Renderer/GraphicLayers/GraphicLayersStack.h"

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

	uint32 VertexArray;
	class IVertexBuffer* VertexBuffer;
	class IIndexBuffer* IndexBuffer;
	class IShader* Shader;
};

// should be define in client
FApplication* CreateApplication();