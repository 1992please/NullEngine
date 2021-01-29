#include "OpenGLBuffers.h"
#include "GL/gl3w.h"

FOpenGLVertexBuffer::FOpenGLVertexBuffer(float* InVertices, uint32 InSize)
{
	glGenBuffers(1, &RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, RendererID);
	glBufferData(GL_ARRAY_BUFFER, InSize, InVertices, GL_STATIC_DRAW);
}

void FOpenGLVertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, RendererID);
}

void FOpenGLVertexBuffer::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

FOpenGLVertexBuffer::~FOpenGLVertexBuffer()
{
	glDeleteBuffers(1, &RendererID);
}

FOpenGLIndexBuffer::FOpenGLIndexBuffer(uint32* InIndices, uint32 InCount)
	: Count(InCount)
{
	glGenBuffers(1, &RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, InCount * sizeof(uint32), InIndices, GL_STATIC_DRAW);
}

void FOpenGLIndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
}

void FOpenGLIndexBuffer::UnBind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

FOpenGLIndexBuffer::~FOpenGLIndexBuffer()
{
	glDeleteBuffers(1, &RendererID);
}
