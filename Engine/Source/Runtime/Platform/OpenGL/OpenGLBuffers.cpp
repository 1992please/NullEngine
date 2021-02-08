#include "NullPCH.h"
#include "OpenGLBuffers.h"
#include "GL/gl3w.h"

FOpenGLVertexBuffer::FOpenGLVertexBuffer(float* InVertices, uint32 InSize)
{
	NE_PROFILE_FUNCTION();

	glGenBuffers(1, &RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, RendererID);
	glBufferData(GL_ARRAY_BUFFER, InSize, InVertices, GL_STATIC_DRAW);
}

void FOpenGLVertexBuffer::Bind() const
{
	NE_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, RendererID);
}

void FOpenGLVertexBuffer::UnBind() const
{
	NE_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

FOpenGLVertexBuffer::~FOpenGLVertexBuffer()
{
	NE_PROFILE_FUNCTION();

	glDeleteBuffers(1, &RendererID);
}

FOpenGLIndexBuffer::FOpenGLIndexBuffer(uint32* InIndices, uint32 InCount)
	: Count(InCount)
{
	NE_PROFILE_FUNCTION();

	glGenBuffers(1, &RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, InCount * sizeof(uint32), InIndices, GL_STATIC_DRAW);
}

void FOpenGLIndexBuffer::Bind() const
{
	NE_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
}

void FOpenGLIndexBuffer::UnBind() const
{
	NE_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

FOpenGLIndexBuffer::~FOpenGLIndexBuffer()
{
	NE_PROFILE_FUNCTION();

	glDeleteBuffers(1, &RendererID);
}
