#pragma once
#include "Core/CoreTypes.h"
#include "Renderer/Components/Buffers.h"

class FOpenGLVertexBuffer : public IVertexBuffer
{
public:
	FOpenGLVertexBuffer(float* InVertices, uint32 InSize);
	FOpenGLVertexBuffer(uint32 InSize);
    virtual void Bind() const override;
    virtual void UnBind() const override;
	virtual void SetLayout(const FBufferLayout& InLayout) override { Layout = InLayout; }
	virtual void SetData(const void* InData, uint32 InSize) override;

	virtual const FBufferLayout& GetLayout() const override { return Layout; }
	virtual ~FOpenGLVertexBuffer();
private:
	uint32 RendererID;
	FBufferLayout Layout;
};

class FOpenGLIndexBuffer : public IIndexBuffer
{
public:
    FOpenGLIndexBuffer(uint32* InIndices, uint32 InCount);
    virtual void Bind() const override;
    virtual void UnBind() const override;
	virtual uint32 GetCount() const override { return Count; }
	virtual ~FOpenGLIndexBuffer();
private:
	uint32 RendererID;
	uint32 Count;
};