#include "NullPCH.h"
#include "Texture.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Renderer/Components/RendererAPI.h"

ITexture2D* ITexture2D::Create(uint32 InWidth, uint32 InHeight)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_CHECK_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLTexture2D(InWidth, InHeight);
	}
	NE_CHECK_F(false, "Unknown Renderer API!!");
	return nullptr;
}

ITexture2D* ITexture2D::Create(const char* InPath)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_CHECK_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLTexture2D(InPath);
	}
	NE_CHECK_F(false, "Unknown Renderer API!!");
	return nullptr;
}
