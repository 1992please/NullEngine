#include "NullPCH.h"
#include "GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"

IGraphicsContext* IGraphicsContext::Create(void* InWindow)
{
	return new FOpenGlContext(InWindow);
}
