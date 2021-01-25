#include "Application.h"
#include "Rendering/ImGui/ImGuiLayer.h"
#include "Core/Application/ApplicationInput.h"
#include "Core/Application/ApplicationWindow.h"

FApplication* FApplication::Instance = nullptr;

FApplication::FApplication()
{
	NE_ASSERT_F(!Instance, "Application already exists!");
	Instance = this;

	pWindow = IApplicationWindow::Create();
	pWindow->WindowEventCallback.BindRaw(this, &FApplication::OnEvent);
	bRunning = true;
	ImGuiLayer = new FImGuiLayer();
	GraphicLayerStack.PushOverlay(ImGuiLayer);
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

		ImGuiLayer->Begin();
		for (FGraphicLayer* Layer : GraphicLayerStack)
		{
			Layer->OnImGuiUpdate();
		}
		ImGuiLayer->End();

		if (IApplicationInput::IsKeyPressed(NE_KEY_BACKSPACE))
		{
			NE_LOG("BACKSPACE IS PRESSED")
		}
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
	//NE_CORE_LOG("%s",*InEvent.ToString())
}

void FApplication::OnWindowCloseEvent(FWindowCloseEvent&)
{
	bRunning = false;
}
