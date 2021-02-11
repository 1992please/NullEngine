#include "NullPCH.h"
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

const uint32 MaxQuads = 10000;
const uint32 MaxVerteces = MaxQuads * 4;
const uint32 MaxIndeces = MaxQuads * 6;

struct FQuadVertex
{
	FVector Position;
	FLinearColor Color;
	FVector2 TexCoords;
	//TODO: texId
};

struct FRenderer2DStorage
{
	IVertexArray* QuadVertexArray;
	IShader* TextureShader;
	ITexture2D* WhiteTexture;

	uint32 QuadIndexCount;
	FQuadVertex* QuadVertexBufferBase;
	FQuadVertex* QuadVertexBufferPtr;
};

static FRenderer2DStorage Storage;


void FRenderer2D::Init()
{
	NE_PROFILE_FUNCTION();

	Storage.QuadVertexArray = IVertexArray::Create();
	Storage.TextureShader = IShader::Create("../../Engine/Shaders/TextureShader.glsl");
	//Storage.TextureShader->Bind();
	//Storage.TextureShader->SetInt("u_Texture", 0);
	//Storage.WhiteTexture = ITexture2D::Create(1, 1);
	//Storage.WhiteTexture->SetData(&FColor::White, sizeof(FColor));
	Storage.QuadIndexCount = 0;
	Storage.QuadVertexBufferBase = nullptr;
	Storage.QuadVertexBufferPtr = nullptr;
	//float Verteces[] = {
	//	.5f,    .5f,   0.0f,   1.0,   1.0,
	//	.5f,   -.5f,   0.0f,   1.0,   0.0,
	//	-.5f,  -.5f,   0.0f,   0.0,   0.0,
	//	-.5f,   .5f,   0.0f,   0.0,   1.0,
	//};

	// TODO stack allocation in fucture memory mamagement
	uint32* Indices = new uint32[MaxIndeces];
	uint32 offset = 0;
	for (uint32 i = 0; i < MaxIndeces; i += 6)
	{
		Indices[i + 0] = offset + 0;
		Indices[i + 1] = offset + 1;
		Indices[i + 2] = offset + 2;

		Indices[i + 3] = offset + 2;
		Indices[i + 4] = offset + 3;
		Indices[i + 5] = offset + 0;
		offset += 4;
	}

	IVertexBuffer* QuadVertexBuffer = IVertexBuffer::Create(MaxVerteces * sizeof(FQuadVertex));
	{
		FBufferLayout Layout = {
			{ "a_Position" ,  EShaderDataType::Float3 },
			{ "a_Color" ,  EShaderDataType::Float4 },
			{ "a_TextureCoords" ,  EShaderDataType::Float2 }
		};

		QuadVertexBuffer->SetLayout(Layout);
	}

	Storage.QuadVertexBufferBase = new FQuadVertex[MaxVerteces];

	IIndexBuffer* QuadIndexBuffer = IIndexBuffer::Create(Indices, MaxIndeces);

	Storage.QuadVertexArray->SetVertexBuffer(QuadVertexBuffer);
	Storage.QuadVertexArray->SetIndexBuffer(QuadIndexBuffer);

	delete[] Indices;
}

void FRenderer2D::Shutdown()
{
	NE_PROFILE_FUNCTION();
	delete Storage.QuadVertexArray;
	delete Storage.TextureShader;
}

void FRenderer2D::BeginScene(const FOrthographicCamera& InCamera)
{
	NE_PROFILE_FUNCTION();

	Storage.TextureShader->Bind();
	Storage.TextureShader->SetMatrix("u_ViewProjection", InCamera.GetViewProjectionMatrix());
	Storage.QuadVertexBufferPtr = Storage.QuadVertexBufferBase;
	Storage.QuadIndexCount = 0;
}



void FRenderer2D::EndScene()
{
	NE_PROFILE_FUNCTION();
	uint32 DataSize = uint32((uint8*)Storage.QuadVertexBufferPtr - (uint8*)Storage.QuadVertexBufferBase);
	Storage.QuadVertexArray->GetVertexBuffer()->SetData(Storage.QuadVertexBufferBase, DataSize);
	Flush();
}


void FRenderer2D::Flush()
{
	NE_PROFILE_FUNCTION();
	FRenderCommand::DrawIndexed(Storage.QuadVertexArray, Storage.QuadIndexCount);
}

void FRenderer2D::DrawQuad(const FVector& InPosition, const FVector2& InSize, const FLinearColor& InColor /*= FLinearColor::White*/, const class ITexture2D* InTexture /*= nullptr*/, const FVector2& Tiling /*= FVector2(1.0f)*/)
{
	NE_PROFILE_FUNCTION();

	Storage.QuadVertexBufferPtr->Position = InPosition;
	Storage.QuadVertexBufferPtr->Color = InColor;
	Storage.QuadVertexBufferPtr->TexCoords = { 0.0f, 0.0f };
	Storage.QuadVertexBufferPtr++;

	Storage.QuadVertexBufferPtr->Position = { InPosition.X + InSize.X, InPosition.Y, InPosition.Z };
	Storage.QuadVertexBufferPtr->Color = InColor;
	Storage.QuadVertexBufferPtr->TexCoords = { 1.0f, 0.0f };
	Storage.QuadVertexBufferPtr++;

	Storage.QuadVertexBufferPtr->Position = { InPosition.X + InSize.X, InPosition.Y + InSize.Y, InPosition.Z };
	Storage.QuadVertexBufferPtr->Color = InColor;
	Storage.QuadVertexBufferPtr->TexCoords = { 1.0f, 1.0f };
	Storage.QuadVertexBufferPtr++;

	Storage.QuadVertexBufferPtr->Position = { InPosition.X, InPosition.Y + InSize.Y, InPosition.Z };
	Storage.QuadVertexBufferPtr->Color = InColor;
	Storage.QuadVertexBufferPtr->TexCoords = { 0.0f, 1.0f };
	Storage.QuadVertexBufferPtr++;

	Storage.QuadIndexCount += 6;
}


void FRenderer2D::DrawRotatedQuad(const FVector& InPosition, const FVector2& InSize, float Rotation, const FLinearColor& InColor, const ITexture2D* InTexture, const FVector2& Tiling)
{
	NE_PROFILE_FUNCTION();

	Storage.TextureShader->SetMatrix("u_Model", FScaleMatrix(InSize) * FRotationTranslationMatrix(FRotator(0.0f, 0.0f, Rotation), InPosition));
	Storage.TextureShader->SetVector4("u_Color", InColor);
	Storage.TextureShader->SetVector2("u_Tiling", Tiling);
	if (InTexture)
	{
		InTexture->Bind();
	}
	else
	{
		Storage.WhiteTexture->Bind();
	}


	Storage.QuadVertexArray->Bind();
	FRenderCommand::DrawIndexed(Storage.QuadVertexArray);
}
