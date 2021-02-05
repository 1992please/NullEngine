#include "Renderer2D.h"
#include "Renderer/Components/VertexArray.h"
#include "Renderer/Components/Shader.h"
#include "Renderer/Components/RenderCommand.h"
#include "Renderer/Components/RendererCamera.h"

struct FRenderer2DStorage
{
	IVertexArray* VertexArray;
	IShader* Shader;
};

static FRenderer2DStorage Storage;

void FRenderer2D::Init()
{
	Storage.VertexArray = IVertexArray::Create();
	Storage.Shader = IShader::Create("../../Engine/Shaders/FlatShader.glsl");

	float Verteces[] = {
		.5f,    .5f,   0.0f,
		.5f,   -.5f,   0.0f,
		-.5f,  -.5f,   0.0f,
		-.5f,   .5f,   0.0f
	};

	uint32 indices[6] = { 0, 1, 2, 0, 2, 3 };

	IVertexBuffer* VertexBuffer = nullptr;
	IIndexBuffer* IndexBuffer = nullptr;

	VertexBuffer = IVertexBuffer::Create(Verteces, sizeof(Verteces));
	{
		FBufferLayout Layout = {
			{ "a_Position" ,  EShaderDataType::Float3 }
		};

		VertexBuffer->SetLayout(Layout);
	}
	IndexBuffer = IIndexBuffer::Create(indices, 6);

	Storage.VertexArray->AddVertexBuffer(VertexBuffer);
	Storage.VertexArray->SetIndexBuffer(IndexBuffer);
}

void FRenderer2D::Shutdown()
{
	delete Storage.VertexArray;
	delete Storage.Shader;
}

void FRenderer2D::BeginScene(const FOrthographicCamera& InCamera)
{
	Storage.Shader->Bind();
	Storage.Shader->SetMatrix("u_ViewProjection", InCamera.GetViewProjectionMatrix());
}

void FRenderer2D::EndScene()
{

}

void FRenderer2D::DrawQuad(const FVector2& InPosition, const FVector2& InSize, const FLinearColor& InColor)
{
	DrawQuad(FVector(InPosition.X, InPosition.Y, 0.0f), InSize, InColor);
}

void FRenderer2D::DrawQuad(const FVector& InPosition, const FVector2& InSize, const FLinearColor& InColor)
{
	Storage.Shader->Bind();
	Storage.Shader->SetVector4("u_Color", InColor);
	Storage.Shader->SetMatrix("u_Model", FMatrix::Identity);
	Storage.VertexArray->Bind();
	FRenderCommand::DrawIndexed(Storage.VertexArray);
}

