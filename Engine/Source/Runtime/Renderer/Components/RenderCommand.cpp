#include "NullPCH.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

IRendererAPI* FRenderCommand::RendererAPI = new FOpenGLRendererAPI;