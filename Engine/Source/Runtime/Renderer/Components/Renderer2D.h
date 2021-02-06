#pragma once
#include "Core/CoreTypes.h"

struct FVector;
struct FVector2;
struct FLinearColor;
class ITexture2D;

class FRenderer2D
{
public:
	static void Init();
	static void Shutdown();
    static void BeginScene(const class FOrthographicCamera& InCamera);
	static void EndScene();

	// Primitives
	static void DrawQuad(const FVector& InPosition, const FVector2& InSize, const FLinearColor& InColor, const ITexture2D* InTexture = nullptr);
};