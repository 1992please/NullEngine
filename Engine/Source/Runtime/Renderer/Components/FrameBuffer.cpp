#include "NullPCH.h"
#include "FrameBuffer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Renderer/Components/RendererAPI.h"

IFrameBuffer* IFrameBuffer::Create(const FFrameBufferInfo& InBufferInfo)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_ASSERT_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLFrameBuffer(InBufferInfo);
	}
	NE_ASSERT_F(false, "Unknown Renderer API!!");
	return nullptr;
}
