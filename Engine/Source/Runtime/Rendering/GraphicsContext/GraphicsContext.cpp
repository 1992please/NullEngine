#include "GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"

IGraphicsContext* IGraphicsContext::Create(void* InWindow)
{
	IGraphicsContext* Out = new FOpenGlContext(InWindow);
	return Out;
}
