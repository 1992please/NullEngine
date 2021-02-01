#include "Application.h"
#include "Renderer/ImGui/ImGuiLayer.h"
#include "Core/Application/ApplicationInput.h"
#include "Core/Application/ApplicationWindow.h"
#include "Renderer/Components/Renderer.h"
#include "Renderer/Components/Shader.h"
#include "Renderer/Components/Buffers.h"
#include "Renderer/Components/VertexArray.h"

FApplication* FApplication::Instance = nullptr;

FApplication::FApplication()
	: Camera(3.2f, 1.8f, 0.0f, 1.0f)
{
	NE_ASSERT_F(!Instance, "Application already exists!");
	Instance = this;

	pWindow = IApplicationWindow::Create();
	pWindow->WindowEventCallback.BindRaw(this, &FApplication::OnEvent);
	bRunning = true;
	ImGuiLayer = new FImGuiLayer();
	GraphicLayerStack.PushOverlay(ImGuiLayer);

	Shader = IShader::Create("../../Engine/Shaders/TestShader.glsl");
	Shader2 = IShader::Create("../../Engine/Shaders/TestShader2.glsl");

	float Verteces[] = {
		0.0f,	0.0f,	0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f,	0.5f,	-0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f,	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f };
	uint32 indices[3] = { 0, 1, 2 };

	float Verteces2[] = {
	0.0f,	0.75f,	0.75f,
	0.0f,	0.75f,	-0.75f,
	0.0f,	-0.75f,	-0.75f,
	0.0f, -0.75f, 0.75f};
	uint32 indices2[6] = { 0, 1, 2, 0, 2, 3 };

	IVertexBuffer* VertexBuffer = nullptr;
	IIndexBuffer* IndexBuffer = nullptr;
	Camera.SetRotation(45.f);
	Camera.SetPosition(FVector(.0f, 0.0f, -0.5f));

	VertexBuffer = IVertexBuffer::Create(Verteces, sizeof(Verteces));
	{
		FBufferLayout Layout = {
			{ "Position" ,  EShaderDataType::Float3 },
			{ "Color" , EShaderDataType::Float4 },
		};

		VertexBuffer->SetLayout(Layout);
	}

	IndexBuffer = IIndexBuffer::Create(indices, 3);

	VertexArray = IVertexArray::Create();
	VertexArray->AddVertexBuffer(VertexBuffer);
	VertexArray->SetIndexBuffer(IndexBuffer);
	VertexArray->UnBind();

	VertexBuffer = IVertexBuffer::Create(Verteces2, sizeof(Verteces2));
	{
		FBufferLayout Layout = {
			{ "Position" ,  EShaderDataType::Float3 }
		};

		VertexBuffer->SetLayout(Layout);
	}
	IndexBuffer = IIndexBuffer::Create(indices2, 6);

	VertexArray2 = IVertexArray::Create();
	VertexArray2->AddVertexBuffer(VertexBuffer);
	VertexArray2->SetIndexBuffer(IndexBuffer);
	VertexArray2->UnBind();

}


FApplication::~FApplication()
{
}

void FApplication::Run()
{
	while (bRunning)
	{
		FRenderCommand::SetClearColor(FLinearColor(0.1f, 0.1f, 0.1f, 1));
		FRenderCommand::Clear();

		FRenderer::BeginScene(Camera);

		FRenderer::Submit(Shader2, VertexArray2);

		FRenderer::Submit(Shader, VertexArray);

		FRenderer::EndScene();

		//Shader2->Bind();
		//VertexArray2->Bind();
		//glDrawElements(GL_TRIANGLES, VertexArray2->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

		//Shader->Bind();
		//VertexArray->Bind();
		//glDrawElements(GL_TRIANGLES, VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);


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
