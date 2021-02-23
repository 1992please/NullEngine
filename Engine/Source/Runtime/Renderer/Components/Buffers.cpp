#include "NullPCH.h"
#include "Buffers.h"
#include "Platform/OpenGL/OpenGLBuffers.h"
#include "Renderer/Components/RendererAPI.h"

IVertexBuffer* IVertexBuffer::Create(float* InVertices, uint32 InSize)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_CHECK_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLVertexBuffer(InVertices, InSize);
	}
	NE_CHECK_F(false, "Unknown Renderer API!!");
	return nullptr;
}

IVertexBuffer* IVertexBuffer::Create(uint32 InSize)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_CHECK_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLVertexBuffer(InSize);
	}
	NE_CHECK_F(false, "Unknown Renderer API!!");
	return nullptr;
}

IIndexBuffer* IIndexBuffer::Create(uint32* InIndices, uint32 InCount)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_CHECK_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLIndexBuffer(InIndices, InCount);
	}
	NE_CHECK_F(false, "Unknown Renderer API!!");
	return nullptr;
}