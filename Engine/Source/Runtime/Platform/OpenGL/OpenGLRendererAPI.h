#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/RendererAPI.h"

class FOpenGLRendererAPI : public IRendererAPI
{
public:
	virtual void Init() override;
	virtual void SetViewport(uint32 X, uint32 Y, uint32 InWidth, uint32 InHeight) override;
	virtual void SetClearColor(const FLinearColor& Color) override;
	virtual void Clear() override;
	virtual void DrawIndexed(const IVertexArray* InVertexArray) override;

};