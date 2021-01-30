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
	NE_ASSERT_F(false, "Unknown Element Type");
	return 0;
}

FOpenGLVertexArray::FOpenGLVertexArray()
	: IndexBuffer(nullptr)
{
	glCreateVertexArrays(1, &RendererID);
}

FOpenGLVertexArray::~FOpenGLVertexArray()
{
	glDeleteVertexArrays(1, &RendererID);
}

void FOpenGLVertexArray::Bind() const
{
	glBindVertexArray(RendererID);
}

void FOpenGLVertexArray::UnBind() const
{
	glBindVertexArray(0);
}

void FOpenGLVertexArray::AddVertexBuffer(IVertexBuffer* InVertexBuffer)
{
	glBindVertexArray(RendererID);
	InVertexBuffer->Bind();
	uint32 EleIndex = 0;
	const FBufferLayout& Layout = InVertexBuffer->GetLayout();
	NE_ASSERT_F(Layout.GetElements().Num() > 0, "Vertex buffer layout is empty.");
	for (const FBufferElement& Ele : Layout)
	{
		glEnableVertexAttribArray(EleIndex);
		glVertexAttribPointer(EleIndex,
			Ele.GetComponentCount(),
			ShaderDataTypeToOpenGLBaseType(Ele.Type),
			Ele.bNormalized ? GL_TRUE : GL_FALSE,
			Layout.GetStride(),
			(const void*)Ele.Offset);

		EleIndex++;
	}

	VertexBuffers.Add(InVertexBuffer);

}

void FOpenGLVertexArray::SetIndexBuffer(IIndexBuffer* InIndexBuffer)
{
	glBindVertexArray(RendererID);
	InIndexBuffer->Bind();
	IndexBuffer = InIndexBuffer;
}

