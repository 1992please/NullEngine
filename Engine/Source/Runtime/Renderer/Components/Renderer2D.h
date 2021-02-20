#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/NullMath.h"

class FRenderer2D
{
public:
	static void Init();
	static void Shutdown();
    static void BeginScene(const struct FSceneView& InSceneView);
	static void EndScene();
	static void Flush();
	static void FlushAndReset();
	// Primitives
	static void DrawQuad(const FVector& InPosition,
		const FVector2& InSize,
		const FLinearColor& InColor = FLinearColor::White,
		const class ITexture2D* InTexture = nullptr,
		const FVector2& Tiling = FVector2(1.0f));

	static void DrawRotatedQuad(const FVector& InPosition, 
		const FVector2& InSize, 
		float Rotation,
		const FLinearColor& InColor = FLinearColor::White, 
		const class ITexture2D* InTexture = nullptr, 
		const FVector2& Tiling = FVector2(1.0f));

	static void DrawQuad(
		const FMatrix& Model,
		const FLinearColor& InColor = FLinearColor::White,
		const class ITexture2D* InTexture = nullptr,
		const FVector2& Tiling = FVector2(1.0f));

	struct FStatistics
	{
		uint32 DrawCalls;
		uint32 QuadCount;

		uint32 VertexCount() { return QuadCount * 4; }
		uint32 IndexCount() { return QuadCount * 6; }
	};
	
	static FStatistics& GetStatistics();
	static void ResetStatistics();
};