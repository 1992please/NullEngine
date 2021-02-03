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


	FORCEINLINE static void InitRenderCommand()
	{
		RendererAPI->InitRendererAPI();
	}
private:
	static IRendererAPI* RendererAPI;
};
