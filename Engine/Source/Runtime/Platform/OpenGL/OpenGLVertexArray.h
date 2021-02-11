#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/VertexArray.h"

class FOpenGLVertexArray : public IVertexArray
{
public:
	FOpenGLVertexArray();
	virtual ~FOpenGLVertexArray();

	virtual void Bind() const override;
	virtual void UnBind() const override;
	virtual void SetVertexBuffer(IVertexBuffer* InVertexBuffer) override;
	virtual void SetIndexBuffer(IIndexBuffer* InIndexBuffer) override;
	virtual IVertexBuffer* GetVertexBuffer() const override { return VertexBuffer; }
	virtual const IIndexBuffer* GetIndexBuffer() const override { return IndexBuffer; }
private:
	uint32 RendererID;
	IVertexBuffer* VertexBuffer;
	IIndexBuffer* IndexBuffer;
};