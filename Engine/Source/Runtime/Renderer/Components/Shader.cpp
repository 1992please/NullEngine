#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer/Components/RendererAPI.h"

IShader* IShader::Create(const FString& InFileName, const FString& InName /*= FString()*/)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_ASSERT_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLShader(InFileName, InName);
	}
	NE_ASSERT_F(false, "Unknown Renderer API!!");
	return nullptr;
}
