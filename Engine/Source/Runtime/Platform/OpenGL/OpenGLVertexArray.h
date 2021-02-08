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
	virtual void AddVertexBuffer(IVertexBuffer* InVertexBuffer) override;
	virtual void SetIndexBuffer(IIndexBuffer* InIndexBuffer) override;
	virtual const TArray<IVertexBuffer*>& GetVertexBuffers() const override { return VertexBuffers; }
	virtual const IIndexBuffer* GetIndexBuffer() const override { return IndexBuffer; }
private:
	uint32 RendererID;
	TArray<IVertexBuffer*> VertexBuffers;
	IIndexBuffer* IndexBuffer;
};