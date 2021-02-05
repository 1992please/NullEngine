#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/RendererAPI.h"

class FRenderCommand
{
public:
	FORCEINLINE static void SetClearColor(const FLinearColor& Color)
	{
		RendererAPI->SetClearColor(Color);
	}

	FORCEINLINE static void Clear()
	{
		RendererAPI->Clear();
	}

	FORCEINLINE static void DrawIndexed(const IVertexArray* InVertexArray)
	{
		RendererAPI->DrawIndexed(InVertexArray);
	}


	FORCEINLINE static void Init()
	{
		RendererAPI->Init();
	}

	FORCEINLINE static void SetViewport(uint32 X, uint32 Y, uint32 InWidth, uint32 InHeight)
	{
		RendererAPI->SetViewport(X, Y, InWidth, InHeight);
	}

private:
	static IRendererAPI* RendererAPI;
};
