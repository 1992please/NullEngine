#include "NullPCH.h"
#include "Renderer2D.h"
#include "Renderer/Components/VertexArray.h"
#include "Renderer/Components/Shader.h"
#include "Renderer/Components/RenderCommand.h"
#include "Renderer/Components/Texture.h"
#include "Core/Math/TranslationMatrix.h"
#include "Core/Math/ScaleMatrix.h"
#include "Core/Math/Color.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Vector2.h"
#include "Engine/Scene/SceneView.h"


const uint32 MaxQuads = 10000;
const uint32 MaxVerteces = MaxQuads * 4;
const uint32 MaxIndeces = MaxQuads * 6;
const uint32 MaxTextureSlots = 32; // TODO get the actual render cap

struct FQuadVertex
{
	FVector Position;
	FLinearColor Color;
	FVector2 TexCoords;
	float TexIndex;
	FVector2 Tiling;
};

struct FRenderer2DStorage
{
	IVertexArray* QuadVertexArray;
	IShader* TextureShader;
	ITexture2D* WhiteTexture;

	uint32 QuadIndexCount;
	FQuadVertex* QuadVertexBufferBase;
	FQuadVertex* QuadVertexBufferPtr;

	const ITexture2D* TextureSlots[MaxTextureSlots];
	uint32 TextureSlotIndex = 1; // 0 = white texture

	FVector4 QuadVertexPositions[4];
	FVector2 QuadTextureCoords[4];

	FRenderer2D::FStatistics Statistics;
};

static FRenderer2DStorage Storage;


void FRenderer2D::Init()
{
	NE_PROFILE_FUNCTION();

	Storage.QuadVertexArray = IVertexArray::Create();
	Storage.TextureShader = IShader::Create("../../Engine/Shaders/TextureShader.glsl");

	Storage.WhiteTexture = ITexture2D::Create(1, 1);
	Storage.WhiteTexture->SetData(&FColor::White, sizeof(FColor));
	Storage.TextureSlots[0] = Storage.WhiteTexture;

	Storage.QuadIndexCount = 0;
	Storage.QuadVertexBufferBase = new FQuadVertex[MaxVerteces];
	Storage.QuadVertexBufferPtr = Storage.QuadVertexBufferBase;

	int32 Samplers[MaxTextureSlots];
	for (int32 i = 0; i < MaxTextureSlots; i++)
		Samplers[i] = i;
	
	Storage.TextureShader->Bind();
	Storage.TextureShader->SetIntArray("u_Textures", Samplers, MaxTextureSlots);

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
	IIndexBuffer* QuadIndexBuffer = IIndexBuffer::Create(Indices, MaxIndeces);
	delete[] Indices;


	IVertexBuffer* QuadVertexBuffer = IVertexBuffer::Create(MaxVerteces * sizeof(FQuadVertex));
	{
		FBufferLayout Layout = {
			{ "a_Position" ,  EShaderDataType::Float3 },
			{ "a_Color" ,  EShaderDataType::Float4 },
			{ "a_TextureCoords" ,  EShaderDataType::Float2 },
			{ "a_TexIndex" ,  EShaderDataType::Float },
			{ "a_Tiling" ,  EShaderDataType::Float2 }
		};

		QuadVertexBuffer->SetLayout(Layout);
	}





	Storage.QuadVertexArray->SetVertexBuffer(QuadVertexBuffer);
	Storage.QuadVertexArray->SetIndexBuffer(QuadIndexBuffer);

	Storage.QuadVertexPositions[0] = { -.5f, -.5f, 0.0f, 1.0f };
	Storage.QuadVertexPositions[1] = { .5f, -.5f, 0.0f, 1.0f };
	Storage.QuadVertexPositions[2] = { .5f, .5f, 0.0f, 1.0f };
	Storage.QuadVertexPositions[3] = { -.5f, .5f, 0.0f, 1.0f };

	Storage.QuadTextureCoords[0] = { 0.0f, 0.0f };
	Storage.QuadTextureCoords[1] = { 1.0f, 0.0f };
	Storage.QuadTextureCoords[2] = { 1.0f, 1.0f };
	Storage.QuadTextureCoords[3] = { 0.0f, 1.0f };

}

void FRenderer2D::Shutdown()
{
	NE_PROFILE_FUNCTION();
	delete Storage.QuadVertexArray;
	delete Storage.TextureShader;
}

void FRenderer2D::BeginScene(const FSceneView& InSceneView)
{
	NE_PROFILE_FUNCTION();

	const FMatrix ViewProjectionMat = InSceneView.ViewMatrix * InSceneView.ProjectionMatrix;

	Storage.TextureShader->Bind();
	Storage.TextureShader->SetMatrix("u_ViewProjection", ViewProjectionMat);

	Storage.QuadVertexBufferPtr = Storage.QuadVertexBufferBase;
	Storage.QuadIndexCount = 0;
	Storage.TextureSlotIndex = 1;

}



void FRenderer2D::EndScene()
{
	NE_PROFILE_FUNCTION();

	Flush();
}

void FRenderer2D::Flush()
{
	NE_PROFILE_FUNCTION();

	if (Storage.QuadIndexCount != 0)
	{
		uint32 DataSize = uint32((uint8*)Storage.QuadVertexBufferPtr - (uint8*)Storage.QuadVertexBufferBase);
		Storage.QuadVertexArray->GetVertexBuffer()->SetData(Storage.QuadVertexBufferBase, DataSize);

		for (uint32 i = 0; i < Storage.TextureSlotIndex; i++)
		{
			Storage.TextureSlots[i]->Bind(i);
		}

		FRenderCommand::DrawIndexed(Storage.QuadVertexArray, Storage.QuadIndexCount);

#if !NE_SHIPPING
		Storage.Statistics.DrawCalls++;
#endif
	}
}

void FRenderer2D::FlushAndReset()
{
	Flush();
	Storage.QuadVertexBufferPtr = Storage.QuadVertexBufferBase;
	Storage.QuadIndexCount = 0;
	Storage.TextureSlotIndex = 1;
}

void FRenderer2D::DrawQuad(const FVector& InPosition, const FVector2& InSize, const FLinearColor& InColor /*= FLinearColor::White*/, const class ITexture2D* InTexture /*= nullptr*/, const FVector2& Tiling /*= FVector2(1.0f)*/)
{
	NE_PROFILE_FUNCTION();
	const FMatrix Model = FScaleMatrix(InSize) * FTranslationMatrix(InPosition);

	DrawQuad(Model, InColor, InTexture, Tiling);
}




void FRenderer2D::DrawRotatedQuad(const FVector& InPosition, const FVector2& InSize, float Rotation, const FLinearColor& InColor, const ITexture2D* InTexture, const FVector2& Tiling)
{
	NE_PROFILE_FUNCTION();
	const FMatrix Model = FScaleMatrix(InSize) * FRotationTranslationMatrix(FRotator(0.0f, 0.0f, Rotation), InPosition);
	DrawQuad(Model, InColor, InTexture, Tiling);
}

void FRenderer2D::DrawQuad(const FMatrix& Model, const FLinearColor& InColor /*= FLinearColor::White*/, const class ITexture2D* InTexture /*= nullptr*/, const FVector2& Tiling /*= FVector2(1.0f)*/)
{
	NE_PROFILE_FUNCTION();

	if (Storage.QuadIndexCount >= MaxIndeces)
	{
		FlushAndReset();
	}

	uint32 TextureIndex = 0;

	if (InTexture)
	{
		for (uint32 i = 1; i < Storage.TextureSlotIndex; i++)
		{
			if (*Storage.TextureSlots[i] == *InTexture)
			{
				TextureIndex = i;
				break;
			}
		}

		if (TextureIndex == 0)
		{
			TextureIndex = Storage.TextureSlotIndex++;
			Storage.TextureSlots[TextureIndex] = InTexture;
		}
	}

	for (int32 i = 0; i < 4; i++)
	{
		Storage.QuadVertexBufferPtr->Position = Model.TransformFVector4(Storage.QuadVertexPositions[i]);
		Storage.QuadVertexBufferPtr->Color = InColor;
		Storage.QuadVertexBufferPtr->TexCoords = Storage.QuadTextureCoords[i];
		Storage.QuadVertexBufferPtr->TexIndex = (float)TextureIndex;
		Storage.QuadVertexBufferPtr->Tiling = Tiling;
		Storage.QuadVertexBufferPtr++;
	}

	Storage.QuadIndexCount += 6;

#if !NE_SHIPPING
	Storage.Statistics.QuadCount++;
#endif
}

FRenderer2D::FStatistics& FRenderer2D::GetStatistics()
{
	return Storage.Statistics;
}

void FRenderer2D::ResetStatistics()
{
	FMemory::Memzero(&Storage.Statistics, sizeof(Storage.Statistics));
}
