#include "NullPCH.h"
#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer/Components/RendererAPI.h"

IShader* IShader::Create(const FString& InFileName, const FString& InName /*= FString()*/)
{
	switch (IRendererAPI::GetAPI())
	{
		case IRendererAPI::Type_None: NE_CHECK_F(false, "None Renderer API is not implemented yet."); return nullptr;
		case IRendererAPI::Type_OpenGL: return new FOpenGLShader(InFileName, InName);
	}
	NE_CHECK_F(false, "Unknown Renderer API!!");
	return nullptr;
}

void FShaderLibrary::Add(const FString& InName, IShader* InShader)
{
	NE_CHECK_F(!Exists(InName), "Shader already exits");
	Shaders.Add(InName, InShader);
}

void FShaderLibrary::Add(IShader* InShader)
{
	Shaders.Add(InShader->GetName(), InShader);
}

IShader* FShaderLibrary::Load(const FString& InFilePath)
{
	IShader* CurShader = IShader::Create(InFilePath);
	Add(CurShader);
	return CurShader;
}

IShader* FShaderLibrary::Load(const FString& InName, const FString& InFilePath)
{
	IShader* CurShader = IShader::Create(InFilePath, InName);
	Add(CurShader);
	return CurShader;
}
