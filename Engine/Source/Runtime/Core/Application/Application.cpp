#include "Application.h"
#include "Renderer/ImGui/ImGuiLayer.h"
#include "Core/Application/ApplicationInput.h"
#include "Core/Application/ApplicationWindow.h"
#include "GL/gl3w.h"
#include "Renderer/Components/Shader.h"
#include "Renderer/Components/Buffers.h"

FApplication* FApplication::Instance = nullptr;

static GLenum ShaderDataTypeToOpenGLBaseType(FBufferElement::EType InType)
{
	switch (InType)
	{
		case FBufferElement::TYPE_Float:	return GL_FLOAT;
		case FBufferElement::TYPE_Float2:	return GL_FLOAT;
		case FBufferElement::TYPE_Float3:	return GL_FLOAT;
		case FBufferElement::TYPE_Float4:	return GL_FLOAT;
		case FBufferElement::TYPE_Mat3:		return GL_FLOAT;
		case FBufferElement::TYPE_Mat4:		return GL_FLOAT;
		case FBufferElement::TYPE_Int:		return GL_INT;
		case FBufferElement::TYPE_Int2:		return GL_INT;
		case FBufferElement::TYPE_Int3:		return GL_INT;
		case FBufferElement::TYPE_Int4:		return GL_INT;
		case FBufferElement::TYPE_Bool:		return GL_BOOL;
	}
	NE_ASSERT_F(false, "Unknown Element Type");
	return 0;
}

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

	float Verteces[] = {
		0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		- 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f };


	VertexBuffer = IVertexBuffer::Create(Verteces, sizeof(Verteces));
	{
		FBufferLayout Layout = {
			{ "Position" , FBufferElement::TYPE_Float3 },
			{ "Color" , FBufferElement::TYPE_Float4 },
		};
		
		VertexBuffer->SetLayout(Layout);
	}

	uint32 EleIndex = 0;
	const FBufferLayout& lLayout = VertexBuffer->GetLayout();
	for (const FBufferElement& Ele : lLayout)
	{
		glEnableVertexAttribArray(EleIndex);
		glVertexAttribPointer(EleIndex, 
			Ele.GetComponentCount(), 
			ShaderDataTypeToOpenGLBaseType(Ele.Type), 
			Ele.bNormalized ? GL_TRUE : GL_FALSE,
			lLayout.GetStride(),
			(const void*) Ele.Offset);

		EleIndex++;
	}




	uint32 indices[3] = { 0, 1, 2 };

	IndexBuffer = IIndexBuffer::Create(indices, 3);

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

		glDrawElements(GL_TRIANGLES, IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

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
