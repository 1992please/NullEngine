#include "Application.h"
#include "Rendering/ImGui/ImGuiLayer.h"
#include "Core/Application/ApplicationInput.h"
#include "Core/Application/ApplicationWindow.h"
#include "GL/gl3w.h"
#include "Rendering/Shader.h"

FApplication* FApplication::Instance = nullptr;

static IShader* Shader;

FApplication::FApplication()
{
	NE_ASSERT_F(!Instance, "Application already exists!");
	Instance = this;

	pWindow = IApplicationWindow::Create();
	pWindow->WindowEventCallback.BindRaw(this, &FApplication::OnEvent);
	bRunning = true;
	ImGuiLayer = new FImGuiLayer();
	GraphicLayerStack.PushOverlay(ImGuiLayer);

	glGenVertexArrays(1, &VertexArray);
	glBindVertexArray(VertexArray);

	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	TArray<FVector> Verteces;
	Verteces.Emplace(0.0f, 0.5f, 0.0f);
	Verteces.Emplace(0.5f, -0.5f, 0.0f);
	Verteces.Emplace(-0.5f, -0.5f, 0.0f);
	int size = Verteces.Num() * sizeof(FVector);
	glBufferData(GL_ARRAY_BUFFER, size, Verteces.GetData(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FVector), nullptr);

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);

	uint32 indices[3] = { 0, 1, 2 };
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 3, indices, GL_STATIC_DRAW);

	Shader = IShader::Create("../../Engine/Shaders/TestShader.glsl");
}


FApplication::~FApplication()
{
}

void FApplication::Run()
{
	while (bRunning)
	{
		Shader->Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

		Shader->Unbind();
		for (FGraphicLayer* Layer : GraphicLayerStack)
		{
			Layer->OnUpdate(0.0f);
		}


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
	//NE_CORE_LOG("%s",*InEvent.ToString())
}

void FApplication::OnWindowCloseEvent(FWindowCloseEvent&)
{
	bRunning = false;
}
