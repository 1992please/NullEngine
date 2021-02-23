#include "NullPCH.h"
#include "OpenGLVertexArray.h"
#include "GL/gl3w.h"

static GLenum ShaderDataTypeToOpenGLBaseType(EShaderDataType InType)
{
	switch (InType)
	{
		case  EShaderDataType::Float:	return GL_FLOAT;
		case  EShaderDataType::Float2:	return GL_FLOAT;
		case  EShaderDataType::Float3:	return GL_FLOAT;
		case  EShaderDataType::Float4:	return GL_FLOAT;
		case  EShaderDataType::Mat3:	return GL_FLOAT;
		case  EShaderDataType::Mat4:	return GL_FLOAT;
		case  EShaderDataType::Int:		return GL_INT;
		case  EShaderDataType::Int2:	return GL_INT;
		case  EShaderDataType::Int3:	return GL_INT;
		case  EShaderDataType::Int4:	return GL_INT;
		case  EShaderDataType::Bool:	return GL_BOOL;

	}
	NE_CHECK_F(false, "Unknown Element Type");
	return 0;
}

FOpenGLVertexArray::FOpenGLVertexArray()
	: IndexBuffer(nullptr)
	, VertexBuffer(nullptr)
{
	NE_PROFILE_FUNCTION();

	glCreateVertexArrays(1, &RendererID);
}

FOpenGLVertexArray::~FOpenGLVertexArray()
{
	NE_PROFILE_FUNCTION();

	glDeleteVertexArrays(1, &RendererID);

	if (VertexBuffer)
	{
		delete VertexBuffer;
	}

	if (IndexBuffer)
	{
		delete IndexBuffer;
	}
}

void FOpenGLVertexArray::Bind() const
{
	NE_PROFILE_FUNCTION();

	glBindVertexArray(RendererID);
}

void FOpenGLVertexArray::UnBind() const
{
	NE_PROFILE_FUNCTION();

	glBindVertexArray(0);

}

void FOpenGLVertexArray::SetVertexBuffer(IVertexBuffer* InVertexBuffer)
{
	NE_PROFILE_FUNCTION();

	glBindVertexArray(RendererID);
	InVertexBuffer->Bind();
	uint32 EleIndex = 0;
	const FBufferLayout& Layout = InVertexBuffer->GetLayout();
	NE_CHECK_F(Layout.GetElements().Num() > 0, "Vertex buffer layout is empty.");
	for (const FBufferElement& Ele : Layout)
	{
		glEnableVertexAttribArray(EleIndex);
		glVertexAttribPointer(EleIndex,
			Ele.GetComponentCount(),
			ShaderDataTypeToOpenGLBaseType(Ele.Type),
			Ele.bNormalized ? GL_TRUE : GL_FALSE,
			Layout.GetStride(),
			(const void*)(size_t)Ele.Offset);

		EleIndex++;
	}

	VertexBuffer = InVertexBuffer;

}

void FOpenGLVertexArray::SetIndexBuffer(IIndexBuffer* InIndexBuffer)
{
	NE_PROFILE_FUNCTION();

	glBindVertexArray(RendererID);
	InIndexBuffer->Bind();
	IndexBuffer = InIndexBuffer;
}
