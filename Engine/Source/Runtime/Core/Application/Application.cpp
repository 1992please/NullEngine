#include "Application.h"
#include "Renderer/ImGui/ImGuiLayer.h"
#include "Core/Application/AppTime.h"
#include "Core/Application/ApplicationInput.h"
#include "Core/Application/ApplicationWindow.h"
#include "Renderer/Components/Renderer.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"

FApplication* FApplication::Instance = nullptr;

FGraphicLayer* GetExternalLayer();

FApplication::FApplication()
	: bMinimized(false)
	, bRunning(true)
{
	NE_ASSERT_F(!Instance, "Application already exists!");
	Instance = this;

	pWindow = IApplicationWindow::Create();
	pWindow->WindowEventCallback.BindRaw(this, &FApplication::OnEvent);
	pWindow->SetVSync(false);

	FRenderer::InitRenderer();

	ImGuiLayer = new FImGuiLayer();
	GraphicLayerStack.PushOverlay(ImGuiLayer);
	GraphicLayerStack.PushLayer(GetExternalLayer());
}


FApplication::~FApplication()
{
}

void FApplication::Run()
{
	while (bRunning)
	{
		const float DeltaTime = (float)FAppTime::GetDeltaTime();

		if (!bMinimized)
		{
			for (FGraphicLayer* Layer : GraphicLayerStack)
			{
				Layer->OnUpdate(DeltaTime);
			}
		}

		ImGuiLayer->Begin();
		for (FGraphicLayer* Layer : GraphicLayerStack)
		{
			Layer->OnImGuiRender();
		}
		ImGuiLayer->End();

		pWindow->OnUpdate();

		FAppTime::UpdateTime();
	}
}

void FApplication::OnEvent(IEvent& InEvent)
{
	switch (InEvent.GetEventType())
	{
		case EventType::WindowResize:	InEvent.bHandled |= OnWindowResizeEvent(static_cast<FWindowResizeEvent&>(InEvent)); break;
		case EventType::WindowClose:	InEvent.bHandled |= OnWindowCloseEvent(static_cast<FWindowCloseEvent&>(InEvent)); break;
	}

	if (InEvent.bHandled)
		return;

	for (FGraphicLayersStack::LayerIterator LayerIt = GraphicLayerStack.end(); LayerIt != GraphicLayerStack.begin(); )
	{
		(*--LayerIt)->OnEvent(InEvent);
		if (InEvent.bHandled)
			break;
	}

}

bool FApplication::OnWindowCloseEvent(FWindowCloseEvent&)
{
	bRunning = false;
	return true;
}

bool FApplication::OnWindowResizeEvent(class FWindowResizeEvent& InWindowResizeEvent)
{
	if (InWindowResizeEvent.GetWidth() == 0 || InWindowResizeEvent.GetHeight() == 0)
	{
		bMinimized = true;
		return false;
	}
	bMinimized = false;
	FRenderer::OnWindowResize(InWindowResizeEvent.GetWidth(), InWindowResizeEvent.GetHeight());

	return false;
}
