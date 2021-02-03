#include "Application.h"
#include "Renderer/ImGui/ImGuiLayer.h"
#include "Core/Application/AppTime.h"
#include "Core/Application/ApplicationInput.h"
#include "Core/Application/ApplicationWindow.h"
#include "Renderer/Components/Renderer.h"
#include "Core/Events/Event.h"
#include "Core/Events/ApplicationEvent.h"


FApplication* FApplication::Instance = nullptr;

FApplication::FApplication()
{
	NE_ASSERT_F(!Instance, "Application already exists!");
	Instance = this;

	pWindow = IApplicationWindow::Create();
	pWindow->WindowEventCallback.BindRaw(this, &FApplication::OnEvent);
	pWindow->SetVSync(false);

	FRenderer::InitRenderer();

	bRunning = true;
	ImGuiLayer = new FImGuiLayer();
	GraphicLayerStack.PushOverlay(ImGuiLayer);
	GraphicLayerStack.PushLayer(GetExternalLayer());
}


FApplication::~FApplication()
{
}

void FApplication::Run()
{
	FAppTime::InitAppTime();
	while (bRunning)
	{
		FRenderCommand::SetClearColor(FLinearColor(0.1f, 0.1f, 0.1f, 1));
		FRenderCommand::Clear();

		const float DeltaTime = (float)FAppTime::GetDeltaTime();

		for (FGraphicLayer* Layer : GraphicLayerStack)
		{
			Layer->OnUpdate(DeltaTime);
		}

		ImGuiLayer->Begin();
		for (FGraphicLayer* Layer : GraphicLayerStack)
		{
			Layer->OnImGuiRender();
		}
		ImGuiLayer->End();

		pWindow->OnUpdate();

		FAppTime::UpdateAppTime();
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
}

void FApplication::OnWindowCloseEvent(FWindowCloseEvent&)
{
	bRunning = false;
}
