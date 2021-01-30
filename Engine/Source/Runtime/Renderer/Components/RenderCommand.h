#pragma once
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


private:
	static IRendererAPI* RendererAPI;
};
