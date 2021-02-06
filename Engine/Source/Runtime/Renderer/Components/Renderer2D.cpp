#include "Renderer2D.h"
#include "Renderer/Components/VertexArray.h"
#include "Renderer/Components/Shader.h"
#include "Renderer/Components/RenderCommand.h"
#include "Engine/RendererCamera.h"
#include "Renderer/Components/Texture.h"
#include "Core/Math/TranslationMatrix.h"
#include "Core/Math/ScaleMatrix.h"
#include "Core/Math/Color.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Vector2.h"

struct FRenderer2DStorage
{
	IVertexArray* VertexArray;
	IShader* TextureShader;
	ITexture2D* WhiteTexture;
};

static FRenderer2DStorage Storage;

void FRenderer2D::Init()
{
	Storage.VertexArray = IVertexArray::Create();
	Storage.TextureShader = IShader::Create("../../Engine/Shaders/TextureShader.glsl");
	Storage.TextureShader->Bind();
	Storage.TextureShader->SetInt("u_Texture", 0);
	Storage.WhiteTexture = ITexture2D::Create(1, 1);
	Storage.WhiteTexture->SetData(&FColor::White, sizeof(FColor));

	float Verteces[] = {
		.5f,    .5f,   0.0f,   1.0,   1.0,
		.5f,   -.5f,   0.0f,   1.0,   0.0,
		-.5f,  -.5f,   0.0f,   0.0,   0.0,
		-.5f,   .5f,   0.0f,   0.0,   1.0,
	};

	uint32 indices[6] = { 2, 1, 0, 3, 2, 0 };

	IVertexBuffer* VertexBuffer = nullptr;
	IIndexBuffer* IndexBuffer = nullptr;

	VertexBuffer = IVertexBuffer::Create(Verteces, sizeof(Verteces));
	{
		FBufferLayout Layout = {
			{ "a_Position" ,  EShaderDataType::Float3 },
			{ "a_TextureCoords" ,  EShaderDataType::Float2 }
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
	delete Storage.TextureShader;
}

void FRenderer2D::BeginScene(const FOrthographicCamera& InCamera)
{
	Storage.TextureShader->Bind();
	Storage.TextureShader->SetMatrix("u_ViewProjection", InCamera.GetViewProjectionMatrix());

}

void FRenderer2D::EndScene()
{

}

void FRenderer2D::DrawQuad(const FVector& InPosition, const FVector2& InSize, const FLinearColor& InColor, const ITexture2D* InTexture)
{
	Storage.TextureShader->SetMatrix("u_Model", FScaleMatrix(InSize) * FTranslationMatrix(InPosition));
	Storage.TextureShader->SetVector4("u_Color", InColor);
	if (InTexture)
	{
		InTexture->Bind();
	}
	else
	{
		Storage.WhiteTexture->Bind();
	}


	Storage.VertexArray->Bind();
	FRenderCommand::DrawIndexed(Storage.VertexArray);
}
