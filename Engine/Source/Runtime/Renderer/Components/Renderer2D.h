#pragma once
#include "Core/CoreTypes.h"
#include "Core/Math/Color.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Vector2.h"

class FRenderer2D
{
public:
	static void Init();
	static void Shutdown();
    static void BeginScene(const class FOrthographicCamera& InCamera);
	static void EndScene();

	// Primitives
	static void DrawQuad(const FVector2& InPosition, const FVector2& InSize, const FLinearColor& InColor);
	static void DrawQuad(const FVector& InPosition, const FVector2& InSize, const FLinearColor& InColor);
};