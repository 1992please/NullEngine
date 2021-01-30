#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/RendererAPI.h"

class FOpenGLRendererAPI : public IRendererAPI
{
public:
	virtual void SetClearColor(const FLinearColor& Color) override;
	virtual void Clear() override;
	virtual void DrawIndexed(const IVertexArray* InVertexArray) override;

};