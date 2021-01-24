#include "Application.h"
#include "Rendering/ImGui/ImGuiLayer.h"

FApplication* FApplication::Instance = nullptr;

FApplication::FApplication()
{
	NE_ASSERT_F(!Instance, "Application already exists!");
	Instance = this;

	pWindow = FWindow::Create();
	pWindow->WindowEventCallback.BindRaw(this, &FApplication::OnEvent);
	bRunning = true;

	GraphicLayerStack.PushLayer(new FImGuiLayer);
}


FApplication::~FApplication()
{
}

void FApplication::Run()
{
	while (bRunning)
	{
		for (FGraphicLayer* Layer : GraphicLayerStack)
		{
			Layer->OnUpdate(0.0f);
		}
		pWindow->OnUpdate();
	}
}

void FApplication::OnEvent(IEvent& InEvent)
{
	for (FGraphicLayersStack::LayerIterator LayerIt = GraphicLayerStack.end(); LayerIt != GraphicLayerStack.begin(); )
	{
		(*--LayerIt)->OnEvent(InEvent);
		if (InEvent.bHandled)
			break;
	}
	switch (InEvent.GetEventType())
	{
		case EventType::WindowClose:
		{
			OnWindowCloseEvent(*(FWindowCloseEvent*)&InEvent);
		}
	}
	NE_CORE_LOG("%s",*InEvent.ToString())
}

void FApplication::OnWindowCloseEvent(FWindowCloseEvent&)
{
	bRunning = false;
}
