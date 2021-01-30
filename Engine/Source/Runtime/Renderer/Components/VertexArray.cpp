#include "VertexArray.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

IVertexArray* IVertexArray::Create()
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_ASSERT_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLVertexArray();
	}
	NE_ASSERT_F(false, "Unknown Renderer API!!");
	return nullptr;
}
